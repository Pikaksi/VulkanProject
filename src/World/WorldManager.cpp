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

EntityID WorldManager::generateEntity(glm::ivec3 chunkLocation, glm::ivec3 blockLocation, uint64_t componentBitmask)
{
	EntityID entityID = entityManager.createEntity(componentBitmask);
	blockEntities[chunkLocation][blockLocation] = entityID;
	return entityID;
}

