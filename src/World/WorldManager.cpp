#include "assertm.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/vec3.hpp"

#include "World/WorldManager.hpp"
#include "World/WorldGeneration/ChunkGenerator.hpp"
#include "DebugMenu.hpp"

void WorldManager::tryGeneratingNewChunk(glm::i32vec3 chunkLocation,
                                         std::vector<glm::ivec3>& chunksToRenderAgain,
                                         ChunkRenderer& chunkRenderer)
{
    if (chunks.contains(chunkLocation)) {
        return;
    }
    generateChunk(chunkLocation, this, chunksToRenderAgain, chunkRenderer);
    debugMenuGlobals.chunksGenerated = chunks.size();

    debugMenuGlobals.chunksGeneratedCompressed = 0;
    debugMenuGlobals.blockSizeTotal = 0;
    for (auto& [key, chunk] : chunks) {
        if (chunk.containsDifferentBlocks) {
            debugMenuGlobals.blockSizeTotal += chunk.blocks.size();
            continue;
        }
        assertm(chunk.blocks.size() == 1, "chunk was not compressed");
        debugMenuGlobals.blockSizeTotal += chunk.blocks.size();
        debugMenuGlobals.chunksGeneratedCompressed += 1;
    }
}

EntityID WorldManager::generateEntity(glm::ivec3 chunkLocation, glm::ivec3 blockLocation, uint64_t componentBitmask)
{
    EntityID entityID = entityManager.createEntity(componentBitmask);
    blockEntities[chunkLocation][blockLocation] = entityID;
    return entityID;
}
