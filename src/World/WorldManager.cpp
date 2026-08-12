#define GLM_ENABLE_EXPERIMENTAL
#include "glm/vec3.hpp"

#include "World/WorldManager.hpp"
#include "World/WorldGeneration/ChunkGenerator.hpp"

void WorldManager::tryGeneratingNewChunk(glm::i32vec3 chunkLocation,
                                         std::vector<glm::ivec3>& chunksToRenderAgain,
                                         ChunkRenderer& chunkRenderer)
{
    if (chunks.contains(chunkLocation)) {
        return;
    }
    generateChunk(chunkLocation, this, chunksToRenderAgain, chunkRenderer);
}

EntityID WorldManager::generateEntity(glm::ivec3 chunkLocation, glm::ivec3 blockLocation, uint64_t componentBitmask)
{
    EntityID entityID = entityManager.createEntity(componentBitmask);
    blockEntities[chunkLocation][blockLocation] = entityID;
    return entityID;
}
