#pragma once

#include "Entity.hpp"
#include "EntityCreator.hpp"

#include <vector>

const uint64_t inventoryComponentBitmask = 0b1 << 0;
const uint64_t blockNetworkComponentBitmask = 0b1 << 1;

class ComponentPool
{
    virtual ~ComponentPool();
    virtual void removeComponent(uint32_t entityID) = 0;
    virtual void addComponent(uint32_t entityID) = 0;
};

class InventoryComponentPool : public ComponentPool
{
    std::vector<Inventory> inventoryComponents;
    std::vector<uint32_t> componentOwners;

    virtual void removeComponent(uint32_t entityID)
    {

    }
    virtual void addComponent(uint32_t entityID) = 0;
};

struct Entity
{
    std::bitset<64> componentBitset;
    std::vector<uint32_t> componentIndecies;

    Entity(uint64_t componentBitmask, std::vector<uint32_t> componentIndecies)
        : componentIndecies(componentIndecies)
    {
        componentBitset |= componentBitmask;
        componentIndecies.resize(componentBitset.count()); // make sure the vector is as small as possible
    }
};


std::vector<Inventory> inventoryComponents;
std::vector<Entity> entities;
