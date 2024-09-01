#pragma once

#include <vector>
#include <bitset>

#include "Components.hpp"
#include "Inventory/Inventory.hpp"

typedef uint32_t EntityID;

enum class EntityPreset
{
    inventory = 0,
    network = 1,
    inventoryAndNetwork = 2
};

class Entity
{
public:
    std::bitset<64> componentBitset;
    uint32_t componentIndecies[64];

    Entity() {};

    template<typename T>
    T& getComponent();
};

class EntityManager
{
public:
    std::vector<Entity> entities;
    std::vector<EntityID> freeEntities; 

    void addEntity(uint64_t componentBitmask);
    void deleteEntity(EntityID entityID);
};

extern EntityManager entityManager;

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

    // Creates a new component and marks that the component is owned by the entity.
    // Returns the index of the new component.
    template<typename T>
    uint32_t addComponent(EntityID entityID)
    {
        constexpr int componentPoolIndex = getComponentIndex<T>();
        std::vector<T>* componentPool = static_cast<std::vector<T>*>(componentPools[componentPoolIndex]);
        componentPool->push_back(T());

        componentOwners[componentPoolIndex].push_back(entityID);

        return componentPool->size() - 1;
    }

    template<typename T>
    std::vector<T> getComponentPool()
    {
        constexpr int componentPoolIndex = getComponentIndex<T>();
        return static_cast<std::vector<T>*>(componentPools[componentPoolIndex]);
    }

    template<typename T>
    void deleteComponent(uint32_t componentIndexToDelete)
    {
        constexpr int componentPoolIndex = getComponentIndex<T>();
        std::vector<T>* componentPool = static_cast<std::vector<T>*>(componentPools[componentPoolIndex]);

        std::swap((*componentPool)[componentIndexToDelete], componentPool->back());
        componentPool->pop_back();

        entityManager.entities[componentOwners[componentPoolIndex].back()].componentIndecies[componentPoolIndex] = componentIndexToDelete;
        std::swap(componentOwners[componentPoolIndex][componentIndexToDelete], componentOwners[componentPoolIndex].back());
        componentOwners[componentPoolIndex].pop_back();
    }
};

extern ComponentPoolManager componentPoolManager;

template<typename T>
T& Entity::getComponent()
{
    constexpr int componentPoolIndex = getComponentIndex<T>();
    std::vector<T>* componentPool = static_cast<std::vector<T>*>(componentPoolManager.componentPools[componentPoolIndex]);
    return (*componentPool)[componentIndecies[componentPoolIndex]];
}
