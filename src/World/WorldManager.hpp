#pragma once

#include <unordered_map>
#include <vector>
#include <unordered_set>
#include <set>

class ChunkRenderer;

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/vec3.hpp"
#include "glm/gtx/hash.hpp"

#include "Chunk.hpp"
#include "ECS/EntityManager.hpp"
#include "ChunkRenderer.hpp"

class WorldManager
{
public:
	std::unordered_map<glm::i32vec3, Chunk> chunks;
	std::unordered_map<glm::i32vec3, Chunk> ungeneratedStructures;
	std::unordered_map<glm::ivec3, std::unordered_map<glm::ivec3, EntityID>> blockEntities;

	WorldManager() {}

	void tryGeneratingNewChunk(glm::i32vec3 chunkLocation, std::unordered_set<glm::ivec3>& chunksToRerender, ChunkRenderer& chunkRenderer);
	EntityID generateEntity(glm::ivec3 chunkLocation, glm::ivec3 blockLocation, uint64_t componentBitmask);
};
