#pragma once

#include <vector>
#include <bitset>

#include "Components.hpp"
#include "Inventory/Inventory.hpp"

const int inventoryComponentIndex = 0;
const uint64_t inventoryComponentBitmask = 0b1 << inventoryComponentIndex;
const int blockNetworkComponentIndex = 1;
const uint64_t blockNetworkComponentBitmask = 0b1 << blockNetworkComponentIndex;
typedef uint32_t EntityID;

enum class EntityArchetype
{
    inventory = 0,
    network = 1,
    inventoryAndNetwork = 2
};

class ComponentPoolManager
{
public:
    std::vector<void*> componentPools;
    std::vector<std::vector<EntityID>> componentOwners;

    ComponentPoolManager()
    {
        componentPools.push_back((void*) new std::vector<Inventory>());
        componentOwners.push_back(std::vector<uint32_t>());
        componentPools.push_back((void*) new std::vector<BlockNetwork>());
        componentOwners.push_back(std::vector<uint32_t>());
    }

    ~ComponentPoolManager()
    {
        for (int i = 0; i < componentPools.size(); i++) {
            delete componentPools[i];
        }
    }

    template<typename T>
    uint32_t addComponent(EntityID entityID, int componentIndex)
    {
        std::vector<T>* componentPool = static_cast<std::vector<T>*>(componentPools[componentIndex]);
        componentPool->push_back(T());

        componentOwners[componentIndex].push_back(entityID);

        return componentPool->size() - 1;
    }

    template<typename T>
    std::vector<T> getComponentPool(std::bitset<64> componentBitmask)
    {
        int componentIndex = std::_Countr_zero(componentBitmask.to_ullong());

        void* componentPool = componentPools[componentIndex];
        return static_cast<std::vector<T>*>(componentPool);
    }
};

extern ComponentPoolManager componentPoolManager;

class Entity
{
public:
    EntityArchetype entityArchetype;
    std::bitset<64> componentBitset;
    uint32_t componentIndecies[64];

    Entity() {};

    template<typename T>
    T& getComponent(uint64_t componentIndex)
    {
        std::vector<T>* componentPool = static_cast<std::vector<T>*>(componentPoolManager.componentPools[componentIndex]);
        return (*componentPool)[componentIndecies[componentIndex]];
    }

    void deleteEntity()
    {

    }
};

class EntityManager
{
public:
    std::vector<Entity> entities;

    void addEntity(EntityArchetype entityArchetype);
};

extern EntityManager entityManager;
