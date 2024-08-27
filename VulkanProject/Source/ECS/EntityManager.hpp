#pragma once

#include "Entity.hpp"
#include "EntityCreator.hpp"

#include <vector>

const int inventoryComponentIndex = 0;
const uint64_t inventoryComponentBitmask = 0b1 << inventoryComponentIndex;
const int blockNetworkComponentIndex = 1;
const uint64_t blockNetworkComponentBitmask = 0b1 << blockNetworkComponentIndex;
typedef uint32_t entityID;

class ComponentPoolManager
{
    std::vector<void*> componentPools;

    template<typename T>
    std::vector<T> getComponentPool(std::bitset<64> componentBitset)
    {
        componentBitset.
    }
};

struct Entity
{
    std::bitset<64> componentBitset;
    std::vector<entityID> componentIndecies;

    Entity(uint64_t componentBitmask, std::vector<entityID> componentIndecies)
        : componentIndecies(componentIndecies)
    {
        componentBitset |= componentBitmask;
        componentIndecies.resize(componentBitset.count()); // make sure the vector is as small as possible
    }
};


std::vector<Inventory> inventoryComponents;
std::vector<Entity> entities;
