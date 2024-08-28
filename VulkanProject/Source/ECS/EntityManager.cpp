#include "EntityManager.hpp"

EntityManager entityManager;
ComponentPoolManager componentPoolManager;

void EntityManager::addEntity(EntityArchetype entityArchetype)
{
    if (entityArchetype == EntityArchetype::inventory) {
        EntityID entityID = entities.size();

        entities.push_back(Entity());
        Entity& entity = entities.back();

        entity.componentBitset = inventoryComponentBitmask;
        entity.componentIndecies[inventoryComponentIndex] = componentPoolManager.addComponent<Inventory>(entityID, inventoryComponentIndex);
    }
    if (entityArchetype == EntityArchetype::network) {
        EntityID entityID = entities.size();

        entities.push_back(Entity());
        Entity& entity = entities.back();

        entity.componentBitset = blockNetworkComponentBitmask;
        entity.componentIndecies[blockNetworkComponentIndex] = componentPoolManager.addComponent<BlockNetwork>(entityID, blockNetworkComponentIndex);
    }
    if (entityArchetype == EntityArchetype::inventoryAndNetwork) {
        EntityID entityID = entities.size();

        entities.push_back(Entity());
        Entity& entity = entities.back();

        entity.componentBitset = inventoryComponentBitmask | blockNetworkComponentBitmask;
        entity.componentIndecies[inventoryComponentIndex] = componentPoolManager.addComponent<Inventory>(entityID, inventoryComponentIndex);
        entity.componentIndecies[blockNetworkComponentIndex] = componentPoolManager.addComponent<BlockNetwork>(entityID, blockNetworkComponentIndex);
    }
}
