#include "glm/vec3.hpp"

#include "WorldManager.hpp"

#include "World/WorldManager.hpp"
#include "World/WorldGeneration/ChunkGenerator.hpp"


void WorldManager::tryGeneratingNewChunk(glm::i32vec3 chunkLocation, std::unordered_set<glm::ivec3>& chunksToRerender)
{
	if (chunks.contains(chunkLocation)) {
		return;
	}
	generateChunk(chunkLocation, this, chunksToRerender);
}

