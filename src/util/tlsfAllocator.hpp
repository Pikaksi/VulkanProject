#pragma once

#include <cstdint>
#include <unordered_map>

struct TlsfBlockHeader
{
    bool free = true;
    uint64_t location = 0;
    uint64_t size = 0;
    TlsfBlockHeader* nextPhysical = nullptr;
    TlsfBlockHeader* prevPhysical = nullptr;
    TlsfBlockHeader* nextFree = nullptr;
    TlsfBlockHeader* prevFree = nullptr;
};

struct TlsfAllocator
{
    // Layer 2 bucket count is 1 << this var
    static const int32_t layer2Size = 4;
    uint64_t size = 0;
    TlsfBlockHeader* firstPhysicalBlock = nullptr;
    std::unordered_map<uint64_t, TlsfBlockHeader*> usedBlocksFromLocation;

    uint64_t layer1BitMask = 0;
    uint64_t layer2BitMask[64];
    TlsfBlockHeader* layer2Blocks[64][1 << layer2Size];

};

void tlsfInit(TlsfAllocator& allocator, uint64_t size);
uint64_t tlsfAllocate(TlsfAllocator& allocator, uint64_t size);
void tlsfFree(TlsfAllocator& allocator, uint64_t location);
void tlsfDestroy(TlsfAllocator& allocator);
void tlsfFreeAll(TlsfAllocator& allocator);
void tlsfTest();
void tlsfDebugPrint(TlsfAllocator& allocator);
void tlsfCheckHealth(TlsfAllocator& allocator);
