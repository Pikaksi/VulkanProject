#pragma once

#include <unordered_map>
#include <vector>

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/vec3.hpp"
#include "glm/gtx/hash.hpp"

#include "Chunk.hpp"


class WorldManager
{
public:
	std::unordered_map<glm::i32vec3, Chunk> chunks;
	

	WorldManager()
	{
		
	}

	void tryGeneratingNewChunk(glm::i32vec3 chunkLocation);
};