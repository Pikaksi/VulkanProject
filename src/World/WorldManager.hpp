#pragma once

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <queue>

struct ChunkRenderer;

#include "util/vec3hash.hpp"
#include "Chunk.hpp"
#include "ECS/EntityManager.hpp"
#include "ChunkRenderer.hpp"

struct ChunkGenerationTask
{
    glm::i32vec3 loc;
    Chunk chunk;
    std::vector<std::pair<glm::i32vec3, BlockType>> structureBlocks;
    bool done = false;
};

struct WorldManager
{
    std::unordered_map<glm::i32vec3, Chunk, Vec3LocalizedHash> chunks;
    std::unordered_map<glm::i32vec3, Chunk, Vec3LocalizedHash> ungeneratedStructures;
    std::queue<glm::i32vec3> chunksToGenerate;
    std::queue<ChunkGenerationTask*> generationResults;
    std::unordered_set<glm::i32vec3, Vec3LocalizedHash> chunksGenerating;

    std::unordered_map<glm::i32vec3, std::unordered_map<glm::i32vec3, EntityID, Vec3LocalizedHash>, Vec3LocalizedHash>
        blockEntities;

    WorldManager() {}

    void addChunkToGenerate(glm::i32vec3 loc);
    void processChunkGenerationResults();
    //void tryGeneratingNewChunk(glm::i32vec3 chunkLocation, std::vector<glm::ivec3>& chunksToRerender, ChunkRenderer& chunkRenderer);
    EntityID generateEntity(glm::ivec3 chunkLocation, glm::ivec3 blockLocation, uint64_t componentBitmask);
};
