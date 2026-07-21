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

    if (size < (1 << allocator.layer2Size)) {
        size = 1 << allocator.layer2Size;
    }

    int32_t msbTemp = std::bit_width(size) - 1;
    uint64_t add = size + (1 << (msbTemp - allocator.layer2Size)) - 1;

    int32_t msb = std::bit_width(add) - 1;
    int32_t index2 = (add ^ (1 << msb)) >> (msb - allocator.layer2Size);

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
    int32_t index2 = (size ^ (1 << msb)) >> (msb - allocator.layer2Size);

    BucketIndices indices = {msb, index2};
    return indices;
}

void placeEmptyBlockInBuckets(TlsfAllocator& allocator, TlsfBlockHeader* block)
{
    assertm(block != nullptr, "block given was nullptr");
    assertm(block->size != 0, "Tried to add a TlsfBlockHeader of size 0");

    BucketIndices indices = bucketSlot(allocator, block->size);

    allocator.layer1BitMask |= 1 << indices.index1;
    allocator.layer2BitMask[indices.index1] |= 1 << indices.index2;
    TlsfBlockHeader* listFirstElementPtr = allocator.layer2Blocks[indices.index1][indices.index2];
    //std::cout << "index1 = " << indices.index1 << " index2a = " << indices.index2 << std::endl;

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
            allocator.layer2BitMask[indices.index1] &= ~(1 << indices.index2);
            if (allocator.layer2BitMask[indices.index1] == 0) {
                allocator.layer1BitMask &= ~(1 << indices.index1);
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
    for (auto& a : allocator.layer2BitMask) {
        a = 0;
    }
    for (auto& a : allocator.layer2Blocks) {
        for (auto& b : a) {
            b = nullptr;
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

    BucketIndices indices = bucketSlotLarger(allocator, size);

    uint64_t layer1Mask = allocator.layer1BitMask & (~((1 << indices.index1) - 1));
    if (layer1Mask == 0) {
        std::cerr << "TlsfAllocator ran out of memory" << std::endl;
        abort();
    }
    int layer1Index = std::countr_zero(layer1Mask);
    //std::cout << "layer 1 bitmask = " << (std::bitset<64>)allocator.layer1BitMask << " index = " << layer1Index << std::endl;
    if (layer1Index != indices.index1) {
        indices.index2 = 0;
    }
    int layer2Index = std::countr_zero(allocator.layer2BitMask[layer1Index] & (~((1 << indices.index2) - 1)));
    //std::cout << "layer 2 bitmask = " << (std::bitset<64>)allocator.layer2BitMask[layer1Index] << " index = " << layer2Index << std::endl;

    //std::cout << "index1 = " << layer1Index << " index2 = " << layer2Index << std::endl;
    TlsfBlockHeader* emptyBlock = allocator.layer2Blocks[layer1Index][layer2Index];

    assertm(emptyBlock != nullptr, "Memory block found was nullptr");
    assertm(emptyBlock->size >= size, "Found empty memory block with less than required space. has space " << emptyBlock->size << " required " << size);

    if (emptyBlock->nextFree != nullptr) {
        emptyBlock->nextFree->prevFree = nullptr;
        allocator.layer2Blocks[layer1Index][layer2Index] = emptyBlock->nextFree;
    }
    else {
        allocator.layer2Blocks[layer1Index][layer2Index] = nullptr;
        allocator.layer2BitMask[layer1Index] &= ~(1 << layer2Index);
        if (allocator.layer2BitMask[layer1Index] == 0) {
            allocator.layer1BitMask &= ~(1 << layer1Index);
        }
    }

    emptyBlock->free = false;
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

        emptyBlock->size = size;
        emptyBlock->nextPhysical = newEmptyHeader;
    }

    return emptyBlock->location;
}


void tlsfFree(TlsfAllocator& allocator, uint64_t location)
{
    assertm(allocator.usedBlocksFromLocation.contains(location), "Tried to remove memory that was not allocated");

    TlsfBlockHeader* usedBlock = allocator.usedBlocksFromLocation[location];

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
    //std::cout << "Free created " << usedBlock->size << " cont memory" << std::endl;
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

void tlsfDebugPrint(TlsfAllocator& allocator)
{
    std::cout << "=== Allocator Info ===\n";

    TlsfBlockHeader* block = allocator.firstPhysicalBlock;
    while (block != nullptr) {
        std::cout << "  free = " << block->free << " | location = " << block->location << " | size = " << block->size << "\n";

        block = block->nextPhysical;
    }
    std::cout << std::endl;
}

void tlsfTest()
{
    TlsfAllocator* allocator = new TlsfAllocator;
    tlsfInit(*allocator, 1 << 16);

    uint64_t loc0 = tlsfAllocate(*allocator, 1 << 16);
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

