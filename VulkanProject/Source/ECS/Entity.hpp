#pragma once

#include <vector>
#include <cstdint>
#include <bitset>

struct Entity
{
    std::bitset<64> componentBitset;
    std::vector<uint32_t> componentIndecies;

    Entity(uint64_t componentBitmask)
    {
        componentBitset |= componentBitmask;
        componentIndecies.resize(componentBitset.count());
        
    }
};
