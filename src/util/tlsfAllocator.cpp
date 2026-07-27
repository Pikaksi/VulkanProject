#include <bit>

#include <bitset>
#include <iostream>
#include "tlsfAllocator.hpp"
#include "assertm.hpp"

struct BucketIndices
{
    int32_t index1;
    int32_t index2;
};

BucketIndices bucketSlotLarger(TlsfAllocator& allocator, uint64_t size)
{
    assertm(size != 0, "Tlsf given size is 0");

    if (size < ((uint64_t)1 << allocator.layer2Size)) {
        size = (uint64_t)1 << allocator.layer2Size;
    }

    int32_t msbTemp = std::bit_width(size) - 1;
    uint64_t add = size + ((uint64_t)1 << (msbTemp - allocator.layer2Size)) - 1;

    int32_t msb = std::bit_width(add) - 1;
    int32_t index2 = (add ^ ((uint64_t)1 << msb)) >> (msb - allocator.layer2Size);

    assertm(index2 < ((uint64_t)1 << allocator.layer2Size), "Tlsf gave too big of an index 2 in slotLarger")
        BucketIndices indices = {msb, index2};
    return indices;
}

BucketIndices bucketSlot(TlsfAllocator& allocator, uint64_t size)
{
    assertm(size != 0, "Tlsf given size is 0");

    if (size < (1 << allocator.layer2Size)) {
        size = 1 << allocator.layer2Size;
    }

    int32_t msb = std::bit_width(size) - 1;
    int32_t index2 = (size ^ ((uint64_t)1 << msb)) >> (msb - allocator.layer2Size);

    assertm(index2 < (1 << allocator.layer2Size), "Tlsf gave too big of an index 2 in slot")
        BucketIndices indices = {msb, index2};
    return indices;
}

void placeEmptyBlockInBuckets(TlsfAllocator& allocator, TlsfBlockHeader* block)
{
    assertm(block != nullptr, "block given was nullptr");
    assertm(block->size != 0, "Tried to add a TlsfBlockHeader of size 0");

    BucketIndices indices = bucketSlot(allocator, block->size);

    allocator.layer1BitMask |= (uint64_t)1 << indices.index1;
    allocator.layer2BitMask[indices.index1] |= (uint64_t)1 << indices.index2;
    TlsfBlockHeader* listFirstElementPtr = allocator.layer2Blocks[indices.index1][indices.index2];

    if (listFirstElementPtr != nullptr) {
        block->nextFree = listFirstElementPtr;
        listFirstElementPtr->prevFree = block;
    }
    allocator.layer2Blocks[indices.index1][indices.index2] = block;
}

void removeFromPhysicalLinkedList(TlsfBlockHeader* block)
{
    if (block->nextPhysical != nullptr) {
        block->nextPhysical->prevPhysical = block->prevPhysical;
    }
    if (block->prevPhysical != nullptr) {
        block->prevPhysical->nextPhysical = block->nextPhysical;
    }
}

void removeFromBucketLinkedList(TlsfAllocator& allocator, TlsfBlockHeader* block)
{
    // If the header is the first one in the list me have to change the allocator.layer2Headers
    if (block->prevFree == nullptr) {
        BucketIndices indices = bucketSlot(allocator, block->size);
        allocator.layer2Blocks[indices.index1][indices.index2] = block->nextFree;

        if (block->nextFree == nullptr) {
            allocator.layer2BitMask[indices.index1] &= ~((uint64_t)1 << indices.index2);
            if (allocator.layer2BitMask[indices.index1] == 0) {
                allocator.layer1BitMask &= ~((uint64_t)1 << indices.index1);
            }
        }
    }
    else {
        block->prevFree->nextFree = block->nextFree;
    }

    if (block->nextFree != nullptr) {
        block->nextFree->prevFree = block->prevFree;
    }
}

void tlsfInit(TlsfAllocator& allocator, uint64_t size)
{
    allocator.size = size;
    allocator.layer1BitMask = (uint64_t)0;
    for (auto& a : allocator.layer2BitMask) {
        a = (uint64_t)0;
    }
    for (int i = 0; i < 64; i++) {
        for (int k = 0; k < (1 << allocator.layer2Size); k++) {
            allocator.layer2Blocks[i][k] = nullptr;
        }
    }

    TlsfBlockHeader* firstEmptyBlock = new TlsfBlockHeader;
    firstEmptyBlock->free = true;
    firstEmptyBlock->size = size;
    firstEmptyBlock->location = 0;

    allocator.firstPhysicalBlock = firstEmptyBlock;

    placeEmptyBlockInBuckets(allocator, firstEmptyBlock);
}

uint64_t tlsfAllocate(TlsfAllocator& allocator, uint64_t size)
{
    assertm(size != 0, "Tlsf requested 0 bytes of memory");

    if (size < (1 << allocator.layer2Size)) {
        size = 1 << allocator.layer2Size;
    }

    BucketIndices smallestIndices = bucketSlotLarger(allocator, size);
    BucketIndices foundIndices;

    uint64_t smallestLayer2Mask =
        allocator.layer2BitMask[smallestIndices.index1] & (~(((uint64_t)1 << smallestIndices.index2) - 1));

    if (smallestLayer2Mask == 0) {
        uint64_t layer1Mask = allocator.layer1BitMask & (~(((uint64_t)1 << (smallestIndices.index1 + 1)) - 1));

        if (layer1Mask == 0) {
            std::cerr << "TlsfAllocator ran out of memory" << std::endl;
            abort();
        }

        foundIndices.index1 = std::countr_zero(layer1Mask);
        foundIndices.index2 = std::countr_zero(allocator.layer2BitMask[foundIndices.index1]);
    }
    else {
        foundIndices.index1 = smallestIndices.index1;
        foundIndices.index2 = std::countr_zero(smallestLayer2Mask);
    }

    TlsfBlockHeader* emptyBlock = allocator.layer2Blocks[foundIndices.index1][foundIndices.index2];

    assertm(emptyBlock != nullptr, "Memory block found was nullptr");
    assertm(emptyBlock->size >= size,
            "Found empty memory block with less than required space. has space " << emptyBlock->size << " required "
                                                                                 << size);

    if (emptyBlock->nextFree != nullptr) {
        emptyBlock->nextFree->prevFree = nullptr;
        allocator.layer2Blocks[foundIndices.index1][foundIndices.index2] = emptyBlock->nextFree;
    }
    else {
        allocator.layer2Blocks[foundIndices.index1][foundIndices.index2] = nullptr;
        allocator.layer2BitMask[foundIndices.index1] &= ~((uint64_t)1 << foundIndices.index2);
        if (allocator.layer2BitMask[foundIndices.index1] == 0) {
            allocator.layer1BitMask &= ~((uint64_t)1 << foundIndices.index1);
        }
    }

    emptyBlock->free = false;
    emptyBlock->nextFree = nullptr;
    emptyBlock->prevFree = nullptr;
    assertm(emptyBlock->location <= allocator.size, "tlsf block location is too large");
    allocator.usedBlocksFromLocation.insert(std::make_pair(emptyBlock->location, emptyBlock));

    // If need to split block into used and free blocks
    if (emptyBlock->size != size) {
        TlsfBlockHeader* newEmptyHeader = new TlsfBlockHeader;
        newEmptyHeader->free = true;
        newEmptyHeader->location = emptyBlock->location + size;
        newEmptyHeader->size = emptyBlock->size - size;
        newEmptyHeader->prevPhysical = emptyBlock;
        newEmptyHeader->nextPhysical = emptyBlock->nextPhysical;
        placeEmptyBlockInBuckets(allocator, newEmptyHeader);

        if (emptyBlock->nextPhysical != nullptr) {
            emptyBlock->nextPhysical->prevPhysical = newEmptyHeader;
        }

        emptyBlock->size = size;
        emptyBlock->nextPhysical = newEmptyHeader;
    }

    return emptyBlock->location;
}

void tlsfFree(TlsfAllocator& allocator, uint64_t location)
{
    assertm(allocator.usedBlocksFromLocation.contains(location), "Tried to remove memory that was not allocated");
    TlsfBlockHeader* usedBlock = allocator.usedBlocksFromLocation.extract(location).mapped();
    assertm(usedBlock != nullptr, "tlsf free lookup block is null");
    assertm(usedBlock->location <= allocator.size, "tlsf free lookup block location is too large");

    usedBlock->free = true;

    TlsfBlockHeader* prevPhysicalBlock = usedBlock->prevPhysical;
    if (prevPhysicalBlock != nullptr && prevPhysicalBlock->free) {
        if (allocator.firstPhysicalBlock == prevPhysicalBlock) {
            allocator.firstPhysicalBlock = usedBlock;
        }

        removeFromPhysicalLinkedList(prevPhysicalBlock);
        removeFromBucketLinkedList(allocator, prevPhysicalBlock);

        usedBlock->location = prevPhysicalBlock->location;
        usedBlock->size += prevPhysicalBlock->size;

        delete prevPhysicalBlock;
    }

    TlsfBlockHeader* nextPhysicalBlock = usedBlock->nextPhysical;
    if (nextPhysicalBlock != nullptr && nextPhysicalBlock->free) {
        removeFromPhysicalLinkedList(nextPhysicalBlock);
        removeFromBucketLinkedList(allocator, nextPhysicalBlock);

        usedBlock->size += nextPhysicalBlock->size;

        delete nextPhysicalBlock;
    }
    placeEmptyBlockInBuckets(allocator, usedBlock);
}

void tlsfDestroy(TlsfAllocator& allocator)
{
    TlsfBlockHeader* block = allocator.firstPhysicalBlock;
    while (block != nullptr) {
        TlsfBlockHeader* prevBlock = block;
        block = block->nextPhysical;
        delete prevBlock;
    }
    delete &allocator;
}

void tlsfCheckHealth(TlsfAllocator& allocator)
{
    TlsfBlockHeader* block = allocator.firstPhysicalBlock;
    while (block != nullptr) {
        assertm(block->location + block->size <= allocator.size, "tlsf block outside of allocator");

        if (block->free == false) {
            assertm(block->nextFree == nullptr, "tlsf full next not null");
            assertm(block->prevFree == nullptr, "tlsf full prev not null");
        }
        else {
            BucketIndices indices = bucketSlot(allocator, block->size);
            assertm(allocator.layer2Blocks[indices.index1][indices.index2] != nullptr,
                    "nullptr stored where block should be");
        }
        if (block->prevPhysical != nullptr) {
            assertm(block->prevPhysical->location + block->prevPhysical->size == block->location, "tlsf block sizes dont match with prev. location = " << block->location << " size = " << block->size);
        }
        if (block->nextPhysical != nullptr) {
            assertm(block->location + block->size == block->nextPhysical->location, "tlsf block sizes dont match with next. location = " << block->location << " size = " << block->size);
            assertm((block->free && block->nextPhysical->free) == false, "Tlsf two free blocks next to each other. Location = " << block->location);
        }
        else {
            assertm(block->location + block->size == allocator.size, "tlsf total size wrong");
        }

        block = block->nextPhysical;
    }
    for (uint64_t i = 0; i < 64; i++) {
        if ((allocator.layer1BitMask & ((uint64_t)1 << i)) != 0) {
            assertm(allocator.layer2BitMask[i] != 0, "no layer2 membet when 1 had marked");
        }

        for (uint64_t k = 0; k < (1 << allocator.layer2Size); k++) {
            if ((allocator.layer2BitMask[i] & ((uint64_t)1 << k)) != 0) {
                assertm((allocator.layer2Blocks[i][k]) != nullptr, "nullptr where block marked");
            }
            else {
                assertm((allocator.layer2Blocks[i][k]) == nullptr,
                        "nullptr not present when required " << i << " | " << k);
            }
        }
    }
    for (auto& a : allocator.usedBlocksFromLocation) {
        assertm(a.first == a.second->location, "tlsf lookup table locations dont match");
        assertm(a.second->location <= allocator.size, "tlsf lookup table location too large");
    }
}

void tlsfDebugPrint(TlsfAllocator& allocator)
{
    std::cout << "=== Allocator Info ===\n";
    std::cout << "layer1 bit mask = " << (std::bitset<64>)allocator.layer1BitMask << std::endl;

    TlsfBlockHeader* block = allocator.firstPhysicalBlock;
    while (block != nullptr) {
        std::cout << "  free = " << block->free << " | location = " << block->location << " | size = " << block->size
                  << "\n";

        block = block->nextPhysical;
    }
    tlsfCheckHealth(allocator);
}

void tlsfTest()
{
    TlsfAllocator* allocator = new TlsfAllocator;
    tlsfInit(*allocator, (uint64_t)1 << 16);

    tlsfDebugPrint(*allocator);
    uint64_t loc0 = tlsfAllocate(*allocator, (uint64_t)1 << 16);
    tlsfDebugPrint(*allocator);
    tlsfFree(*allocator, loc0);

    uint64_t loc1 = tlsfAllocate(*allocator, 250);
    assertm(loc1 == 0, "tlsf test failed 1");

    uint64_t loc2 = tlsfAllocate(*allocator, 1);
    assertm(loc2 == 250, "tlsf test failed 2");
    tlsfDebugPrint(*allocator);

    tlsfFree(*allocator, loc1);
    tlsfAllocate(*allocator, 251);

    tlsfDebugPrint(*allocator);

    assertm(loc2 != 0, "tlsf test failed 3");

    tlsfFree(*allocator, loc2);
    tlsfAllocate(*allocator, 251);

    tlsfDebugPrint(*allocator);

    TlsfBlockHeader* block = allocator->firstPhysicalBlock;
    assertm(block->free == false, "tlsf test failed 4");
    block = block->nextPhysical;
    assertm(block->free == true, "tlsf test failed 5");
    block = block->nextPhysical;
    assertm(block->free == false, "tlsf test failed 6");
    block = block->nextPhysical;
    assertm(block->free == true, "tlsf test failed 7");
    block = block->nextPhysical;
    assertm(block == nullptr, "tlsf test failed 8");

    tlsfDestroy(*allocator);
}
