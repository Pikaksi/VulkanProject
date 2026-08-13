#pragma once

#include <unordered_map>
#include <vector>

class ChunkRenderer;

#include "util/vec3hash.hpp"
#include "Chunk.hpp"
#include "ECS/EntityManager.hpp"
#include "ChunkRenderer.hpp"

struct WorldManager
{
    std::unordered_map<glm::i32vec3, Chunk, Vec3LocalizedHash> chunks;
    std::unordered_map<glm::i32vec3, Chunk, Vec3LocalizedHash> ungeneratedStructures;
    std::unordered_map<glm::i32vec3, std::unordered_map<glm::i32vec3, EntityID, Vec3LocalizedHash>, Vec3LocalizedHash> blockEntities;

    WorldManager() {}

    void tryGeneratingNewChunk(glm::i32vec3 chunkLocation,
                               std::vector<glm::ivec3>& chunksToRerender,
                               ChunkRenderer& chunkRenderer);
    EntityID generateEntity(glm::ivec3 chunkLocation, glm::ivec3 blockLocation, uint64_t componentBitmask);
};
