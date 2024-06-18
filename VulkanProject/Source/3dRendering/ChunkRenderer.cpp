#include <iostream>

#include "ChunkRenderer.hpp"

void ChunkRenderer::update(VulkanCoreInfo* vulkanCoreInfo, VkCommandPool commandPool, WorldManager& worldManager, VertexBufferManager& vertexBufferManager, glm::i32vec3 playerChunkLocation)
{
	renderNewChunks(vulkanCoreInfo, commandPool, worldManager, vertexBufferManager, playerChunkLocation);
	
	// dont rerender chunks rendering position did not move
	if (peviousPlayerChunkLocation == playerChunkLocation) {
		return;
	}
	derenderChunks(playerChunkLocation, vertexBufferManager);

	int minDistanceToNewChunk = 
		std::max(abs(peviousPlayerChunkLocation.x - playerChunkLocation.x),
				 std::max(abs(peviousPlayerChunkLocation.y - playerChunkLocation.y),
						  abs(peviousPlayerChunkLocation.z - playerChunkLocation.z)));
	int newChunkMinDistance = std::max(0, renderDistance - minDistanceToNewChunk);

	addChunksToBeRendered(playerChunkLocation, newChunkMinDistance);

	peviousPlayerChunkLocation = playerChunkLocation;
}

void ChunkRenderer::addChunksToBeRendered(glm::i32vec3 playerChunkLocation, int newChunkMinDistance)
{
	int cubesChecked = 0;
	//std::cout << "min distance for new chunk = " << newChunkMinDistance << "\n";

	// Render close by chunks first.
	// Checks a hollow box for each distance starting from the smallest.
	for (int distance = newChunkMinDistance; distance < renderDistance; distance++) {
		// x direction with all edges
		for (int z = -distance; z <= distance; z++) {
			for (int y = -distance; y <= distance; y++) {

				cubesChecked += 2;
				tryAddChunksToRender(glm::i32vec3(distance, y, z) + playerChunkLocation);
				tryAddChunksToRender(glm::i32vec3(-distance, y, z) + playerChunkLocation);
			}
		}

		// z direction with not x axis edges
		for (int x = -distance + 1; x <= distance - 1; x++) {
			for (int y = -distance; y <= distance; y++) {

				cubesChecked += 2;
				tryAddChunksToRender(glm::i32vec3(x, y, distance) + playerChunkLocation);
				tryAddChunksToRender(glm::i32vec3(x, y, -distance) + playerChunkLocation);
			}
		}

		// y direction with no edges
		for (int x = -distance + 1; x <= distance - 1; x++) {
			for (int z = -distance + 1; z <= distance - 1; z++) {

				cubesChecked += 2;
				tryAddChunksToRender(glm::i32vec3(x, distance, z) + playerChunkLocation);
				tryAddChunksToRender(glm::i32vec3(x, -distance, z) + playerChunkLocation);
			}
		}
	}
}

void ChunkRenderer::derenderChunks(glm::i32vec3 playerChunkLocation, VertexBufferManager& vertexBufferManager)
{
	int minX = playerChunkLocation.x - renderDistance - extraRangeToDerenderChunk;
	int maxX = playerChunkLocation.x + renderDistance + extraRangeToDerenderChunk;
	int minY = playerChunkLocation.y - renderDistance - extraRangeToDerenderChunk;
	int maxY = playerChunkLocation.y + renderDistance + extraRangeToDerenderChunk;
	int minZ = playerChunkLocation.z - renderDistance - extraRangeToDerenderChunk;
	int maxZ = playerChunkLocation.z + renderDistance + extraRangeToDerenderChunk;

	std::vector<glm::i32vec3> chunksToDerender;

	for (const auto& pair : renderedChunks) {
		if (pair.first.x <= minX || pair.first.x >= maxX ||
			pair.first.y <= minY || pair.first.y >= maxY ||
			pair.first.z <= minZ || pair.first.z >= maxZ)
		{
			//std::cout << "derendered chunk at " << pair.first.x << " " << pair.first.y << " " << pair.first.z << "\n";
			vertexBufferManager.derenderChunk(pair.second);
			chunksToDerender.push_back(pair.first);
		}
	}

	for (const auto& chunkLocation : chunksToDerender) {
		renderedChunks.erase(chunkLocation);
	}
}

void ChunkRenderer::tryAddChunksToRender(glm::i32vec3 chunkLocation)
{
	if (!renderedChunks.contains(chunkLocation)) {
		chunksToRender.push(chunkLocation);
	}
}

void ChunkRenderer::renderNewChunks(VulkanCoreInfo* vulkanCoreInfo, VkCommandPool commandPool, WorldManager& worldManager, VertexBufferManager& vertexBufferManager, glm::i32vec3 playerChunkLocation)
{
	if (chunksToRender.size() == 0) {
		return;
	}
	glm::i32vec3 chunkLocation = chunksToRender.front();
	chunksToRender.pop();

	if (renderedChunks.contains(chunkLocation)) {
		return;
	}

	int minX = playerChunkLocation.x - renderDistance - extraRangeToDerenderChunk;
	int maxX = playerChunkLocation.x + renderDistance + extraRangeToDerenderChunk;
	int minY = playerChunkLocation.y - renderDistance - extraRangeToDerenderChunk;
	int maxY = playerChunkLocation.y + renderDistance + extraRangeToDerenderChunk;
	int minZ = playerChunkLocation.z - renderDistance - extraRangeToDerenderChunk;
	int maxZ = playerChunkLocation.z + renderDistance + extraRangeToDerenderChunk;

	if (chunkLocation.x > minX && chunkLocation.x < maxX &&
		chunkLocation.y > minY && chunkLocation.y < maxY &&
		chunkLocation.z > minZ && chunkLocation.z < maxZ)
	{
		renderChunk(vulkanCoreInfo, commandPool, chunkLocation, worldManager, vertexBufferManager);
	}
}

void ChunkRenderer::renderChunk(VulkanCoreInfo* vulkanCoreInfo, VkCommandPool commandPool, glm::i32vec3 chunkLocation, WorldManager& worldManager, VertexBufferManager& vertexBufferManager)
{
	worldManager.tryGeneratingNewChunk(chunkLocation);
	// generate adjacent chunks so that we dont have to rerender this chunk when they are generated
	chunkLocation.x -= 1;
	worldManager.tryGeneratingNewChunk(chunkLocation);
	chunkLocation.x += 2;
	worldManager.tryGeneratingNewChunk(chunkLocation);
	chunkLocation.x -= 1;

	chunkLocation.y -= 1;
	worldManager.tryGeneratingNewChunk(chunkLocation);
	chunkLocation.y += 2;
	worldManager.tryGeneratingNewChunk(chunkLocation);
	chunkLocation.y -= 1;

	chunkLocation.z -= 1;
	worldManager.tryGeneratingNewChunk(chunkLocation);
	chunkLocation.z += 2;
	worldManager.tryGeneratingNewChunk(chunkLocation);
	chunkLocation.z -= 1;

	//std::cout << "rendering chunk " << chunkLocation.x << " " << chunkLocation.y << " " << chunkLocation.z << "\n";

	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	generateChunkMeshData(worldManager, chunkLocation, vertices, indices);
	if (vertices.size() == 0) {
		return;
	}
	uint32_t memoryBlockPointer = vertexBufferManager.addChunkVertices(vulkanCoreInfo, commandPool, vertices, indices);

	renderedChunks.insert(std::make_pair(chunkLocation, memoryBlockPointer));

}