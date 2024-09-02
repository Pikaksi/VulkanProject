#pragma once

#include <unordered_map>
#include <vector>
#include <unordered_set>
#include <set>

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/vec3.hpp"
#include "glm/gtx/hash.hpp"

#include "Chunk.hpp"
#include "ECS/EntityManager.hpp"

class WorldManager
{
public:
	std::unordered_map<glm::i32vec3, Chunk> chunks;
	std::unordered_map<glm::i32vec3, Chunk> ungeneratedStructures;
	std::unordered_map<glm::ivec3, std::unordered_map<glm::ivec3, EntityID>> blockEntities;

	WorldManager() {}

	void tryGeneratingNewChunk(glm::i32vec3 chunkLocation, std::unordered_set<glm::ivec3>& chunksToRerender);
};