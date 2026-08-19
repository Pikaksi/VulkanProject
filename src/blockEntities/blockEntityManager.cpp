#include "blockEntityManager.hpp"
#include "blockEntity.hpp"

BlockEntityId BlockEntityManager::createEntity(glm::i32vec3 loc, BlockEntityType type)
{
    BlockEntity* entity = nullptr;
    if (type == BlockEntityType::furnace) {
        entity = (BlockEntity*)new BlockEntityFurnace();
    }
    // TODO: Reuse indices
    entities.push_back(entity);
    BlockEntityId id = entities.size() - 1;
    blockEntityLocations.insert(std::make_pair(loc, id));

    return id;
}
