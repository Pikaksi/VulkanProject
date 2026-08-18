#include "BlockType.hpp"
#include "Chunk.hpp"
#include "assertm.hpp"
#include <utility>
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/vec3.hpp"

#include "World/WorldManager.hpp"
#include "World/WorldGeneration/ChunkGenerator.hpp"
#include "DebugMenu.hpp"
#include "assertm.hpp"
#include "threadPool.hpp"

#include <pthread.h>
#include <queue>

void workerGenerateChunk(void* arg)
{
    ChunkGenerationTask* param = (ChunkGenerationTask*)arg;

    generateChunk(param->loc, param->chunk, param->structureBlocks);

    param->done = true;
}

void WorldManager::addChunkToGenerate(glm::i32vec3 loc)
{
    if (chunks.contains(loc)) {
        return;
    }
    if (chunksGenerating.contains(loc)) {
        return;
    }
    chunksGenerating.insert(loc);
    ChunkGenerationTask* param = new ChunkGenerationTask{
        .loc = loc,
        .done = false,
    };
    generationResults.push(param);
    globalThreadPool.addWork(&workerGenerateChunk, (void*)param);
}

void mergeChunk(Chunk& chunk, Chunk& chunkToMerge)
{
    if (chunkToMerge.containsDifferentBlocks) {
        for (int i = 0; i < CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE; i++) {
            if (chunkToMerge.blocks[i] == BlockType::air) continue;
            chunkSetBlock(i, chunkToMerge.blocks[i], chunk);
        }
    }
    else {
        BlockType blockToPlace = chunkToMerge.blocks[0];
        if (blockToPlace == BlockType::air) return;
        for (int i = 0; i < CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE; i++) {
            chunkSetBlock(i, blockToPlace, chunk);
        }
    }
}

void WorldManager::processChunkGenerationResults()
{
    while (generationResults.size() != 0) {
        ChunkGenerationTask* task = generationResults.front();
        if (!task->done) {
            break;
        }
        assertm(!chunks.contains(task->loc), "Generated chunk that already exists");

        if (ungeneratedStructures.contains(task->loc)) {
            mergeChunk(task->chunk, ungeneratedStructures.at(task->loc));
            ungeneratedStructures.extract(task->loc);
        }

        chunks.insert(std::make_pair(task->loc, task->chunk));

        for (auto [blockLocWorld, block] : task->structureBlocks) {
            glm::i32vec3 blockLoc = {alwaysPosModulo(blockLocWorld.x, CHUNK_SIZE),
                                     alwaysPosModulo(blockLocWorld.y, CHUNK_SIZE),
                                     alwaysPosModulo(blockLocWorld.z, CHUNK_SIZE)};
            glm::i32vec3 chunkLoc = getChunkLocation(blockLocWorld);
            if (chunks.contains(chunkLoc)) {
                chunkSetBlock(blockLoc.x, blockLoc.y, blockLoc.z, block, chunks.at(chunkLoc));
            }
            else {
                if (!ungeneratedStructures.contains(chunkLoc)) {
                    ungeneratedStructures.insert(std::make_pair(chunkLoc, Chunk(false)));
                }
                chunkSetBlock(
                    blockLoc.x, blockLoc.y, blockLoc.z, block, ungeneratedStructures.at(chunkLoc));
            }
        }
        generationResults.pop();
    }
}

/*struct Cube
{
    glm::i32vec3 loc;
    glm::i32vec3 size;
};

// two lines from x1->x2 and y1->y2
bool lineIntersect(int x1, int x2, int y1, int y2)
{
    return (x1 <= y1 && y1 <= x2) || (x1 <= y2 && y2 <= x2) || (y1 <= x2 && x2 <= y2);
}

bool cubesIntersect(Cube& cube1, Cube& cube2)
{
    return lineIntersect(cube1.loc.x, cube1.loc.x + cube1.size.x, cube2.loc.x, cube2.loc.x + cube2.size.x) &&
           lineIntersect(cube1.loc.y, cube1.loc.y + cube1.size.y, cube2.loc.y, cube2.loc.y + cube2.size.y) &&
           lineIntersect(cube1.loc.z, cube1.loc.z + cube1.size.z, cube2.loc.z, cube2.loc.z + cube2.size.z);
}*/

/*void WorldManager::tryGeneratingNewChunk(glm::i32vec3 chunkLocation,
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
}*/

EntityID WorldManager::generateEntity(glm::ivec3 chunkLocation, glm::ivec3 blockLocation, uint64_t componentBitmask)
{
    EntityID entityID = entityManager.createEntity(componentBitmask);
    blockEntities[chunkLocation][blockLocation] = entityID;
    return entityID;
}
