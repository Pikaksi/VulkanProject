#include "EntityManager.hpp"

#include <stdexcept>

EntityManager entityManager;
ComponentPoolManager componentPoolManager;

uint32_t addComponent(EntityID entityID, uint64_t componentBitmask)
{
    switch (componentBitmask)
    {
    case inventoryComponentBitmask: return componentPoolManager.addComponent<Inventory>(entityID);
    case blockNetworkComponentBitmask: return componentPoolManager.addComponent<BlockNetwork>(entityID); 
    
    default:
        throw std::runtime_error("tried to add component, but bitmask was not recognized.");
    }
}

void deleteComponent(uint32_t componentIndex, uint64_t componentBitmask)
{
    switch (componentBitmask)
    {
    case inventoryComponentBitmask: componentPoolManager.deleteComponent<Inventory>(componentIndex); break;
    case blockNetworkComponentBitmask: componentPoolManager.deleteComponent<BlockNetwork>(componentIndex); break;
    
    default:
        throw std::runtime_error("tried to delete component, but bitmask was not recognized.");
    }
}

void EntityManager::addEntity(uint64_t componentBitmask)
{
    EntityID entityID;
    if (freeEntities.size() == 0) {
        entityID = entities.size();
        entities.push_back(Entity());
    }
    else {
        entityID = freeEntities.back();
        freeEntities.pop_back();
    }
    Entity& entity = entities[entityID];

    while (componentBitmask != 0) {
        uint64_t component = componentBitmask & (0 - componentBitmask); // get only lsb

        entity.componentBitset |= component;
        entity.componentIndecies[getComponentIndex(component)] = addComponent(entityID, component);

        componentBitmask &= componentBitmask - 1; // remove lsb and loop to next
    }
}

void EntityManager::deleteEntity(EntityID entityID)
{
    freeEntities.push_back(entityID);

    uint64_t componentBitmask = entities[entityID].componentBitset.to_ullong();
    while (componentBitmask != 0) {
        uint64_t singleComponentBitmask = componentBitmask & (0 - componentBitmask); // get only lsb

        deleteComponent(entities[entityID].componentIndecies[getComponentIndex(singleComponentBitmask)], singleComponentBitmask);

        componentBitmask &= componentBitmask - 1; // remove lsb
    }

}