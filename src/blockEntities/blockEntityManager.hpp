#pragma once

#include "glm/vec3.hpp"

#include <cinttypes>
#include <unordered_map>
#include <vector>

#include "vec3hash.hpp"
#include "blockEntity.hpp"
#include "BlockType.hpp"

typedef uint32_t BlockEntityId;

struct BlockEntityManager
{
    std::vector<BlockEntity*> entities;
    std::unordered_map<glm::i32vec3, BlockEntityId, Vec3LocalizedHash> blockEntityLocations;

    BlockEntityId createEntity(glm::i32vec3 loc, BlockEntityType blockType);
};
