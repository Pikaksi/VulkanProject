#include "glm/vec3.hpp"

#include "WorldManager.hpp"

#include "World/WorldManager.hpp"
#include "World/WorldGeneration/ChunkGenerator.hpp"


void WorldManager::tryGeneratingNewChunk(glm::i32vec3 chunkLocation)
{
	if (chunks.contains(chunkLocation)) {
		return;
	}
	chunks[chunkLocation] = generateChunk(chunkLocation);
}