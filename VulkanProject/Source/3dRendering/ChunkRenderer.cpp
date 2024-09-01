#include <iostream>
#include <chrono>

#include "ChunkRenderer.hpp"
#include "BinaryGreedyMesher.hpp"

// Derendering chunks will always happen before rendering new ones.
void ChunkRenderer::update(VulkanCoreInfo& vulkanCoreInfo, VkCommandPool commandPool, WorldManager& worldManager, VertexBufferManager& vertexBufferManager, glm::i32vec3 playerChunkLocation)
{
	addQueuedChunkMeshes(vulkanCoreInfo, commandPool, worldManager, vertexBufferManager, playerChunkLocation);
	
	// Don't rerender chunks rendering position did not move.
	if (peviousPlayerChunkLocation == playerChunkLocation) {
		return;
	}
	derenderChunksOutOfRenderdistance(playerChunkLocation, vertexBufferManager);

	int minDistanceToNewChunk = 
		std::max(abs(peviousPlayerChunkLocation.x - playerChunkLocation.x),
				 std::max(abs(peviousPlayerChunkLocation.y - playerChunkLocation.y),
						  abs(peviousPlayerChunkLocation.z - playerChunkLocation.z)));
	int newChunkMinDistance = std::max(0, renderDistance - minDistanceToNewChunk);

	renderNewChunksInRenderdistance(playerChunkLocation, newChunkMinDistance);

	peviousPlayerChunkLocation = playerChunkLocation;
}

void ChunkRenderer::renderNewChunksInRenderdistance(glm::i32vec3 playerChunkLocation, int newChunkMinDistance)
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
				tryAddChunkToRender(glm::i32vec3(distance, y, z) + playerChunkLocation);
				tryAddChunkToRender(glm::i32vec3(-distance, y, z) + playerChunkLocation);
			}
		}

		// z direction with not x axis edges
		for (int x = -distance + 1; x <= distance - 1; x++) {
			for (int y = -distance; y <= distance; y++) {

				cubesChecked += 2;
				tryAddChunkToRender(glm::i32vec3(x, y, distance) + playerChunkLocation);
				tryAddChunkToRender(glm::i32vec3(x, y, -distance) + playerChunkLocation);
			}
		}

		// y direction with no edges
		for (int x = -distance + 1; x <= distance - 1; x++) {
			for (int z = -distance + 1; z <= distance - 1; z++) {

				cubesChecked += 2;
				tryAddChunkToRender(glm::i32vec3(x, distance, z) + playerChunkLocation);
				tryAddChunkToRender(glm::i32vec3(x, -distance, z) + playerChunkLocation);
			}
		}
	}
}

void ChunkRenderer::derenderChunksOutOfRenderdistance(glm::i32vec3 playerChunkLocation, VertexBufferManager& vertexBufferManager)
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
			vertexBufferManager.freeWorldVerticesMemory(pair.second);
			chunksToDerender.push_back(pair.first);
		}
	}

	for (const auto& chunkLocation : chunksToDerender) {
		renderedChunks.erase(chunkLocation);
	}
}

void ChunkRenderer::rerenderChunk(glm::i32vec3 chunkLocation)
{
	for (int i = 0; i < chunksToRerender.size(); i++) {
		if (chunksToRerender[i] == chunkLocation) {
			return;
		}
	}
	chunksToRerender.push_back(chunkLocation);
}

void ChunkRenderer::tryAddChunkToRender(glm::i32vec3 chunkLocation)
{
	if (!renderedChunks.contains(chunkLocation)) {
		chunksToRender.push(chunkLocation);
	}
}

bool ChunkRenderer::chunkIsInRenderDistance(glm::i32vec3 playerChunkLocation, glm::i32vec3 chunkLocation)
{
	int minX = playerChunkLocation.x - renderDistance;
	int maxX = playerChunkLocation.x + renderDistance;
	int minY = playerChunkLocation.y - renderDistance;
	int maxY = playerChunkLocation.y + renderDistance;
	int minZ = playerChunkLocation.z - renderDistance;
	int maxZ = playerChunkLocation.z + renderDistance;

	return chunkLocation.x >= minX && chunkLocation.x <= maxX
		&& chunkLocation.y >= minY && chunkLocation.y <= maxY
		&& chunkLocation.z >= minZ && chunkLocation.z <= maxZ;
}

void ChunkRenderer::addQueuedChunkMeshes(VulkanCoreInfo& vulkanCoreInfo, VkCommandPool commandPool, WorldManager& worldManager, VertexBufferManager& vertexBufferManager, glm::i32vec3 playerChunkLocation)
{
	for (int i = 0; i < chunksToRerender.size(); i++) {
		glm::i32vec3 chunkLocation = chunksToRerender[i];
		if (chunkIsInRenderDistance(playerChunkLocation, chunkLocation)) {

			if (renderedChunks.contains(chunkLocation)) {
				uint32_t memoryLocation = renderedChunks.at(chunkLocation);
				vertexBufferManager.freeWorldVerticesMemory(memoryLocation);
				renderedChunks.erase(chunkLocation);
			} 
			renderChunk(vulkanCoreInfo, commandPool, chunkLocation, worldManager, vertexBufferManager);
		}
	}
	chunksToRerender.clear();

	if (chunksToRender.size() == 0) {
		return;
	}
	glm::i32vec3 chunkLocation = chunksToRender.front();
	chunksToRender.pop();

	if (renderedChunks.contains(chunkLocation)) {
		return;
	}

	if (chunkIsInRenderDistance(playerChunkLocation, chunkLocation)) {
		renderChunk(vulkanCoreInfo, commandPool, chunkLocation, worldManager, vertexBufferManager);
	}
}

void ChunkRenderer::renderChunk(VulkanCoreInfo& vulkanCoreInfo, VkCommandPool commandPool, glm::i32vec3 chunkLocation, WorldManager& worldManager, VertexBufferManager& vertexBufferManager)
{
	std::unordered_set<glm::ivec3> chunksToRerender;
	worldManager.tryGeneratingNewChunk(chunkLocation, chunksToRerender);
	// generate adjacent chunks so that we dont have to rerender this chunk when they are generated
	chunkLocation.x -= 1;
	worldManager.tryGeneratingNewChunk(chunkLocation, chunksToRerender);
	chunkLocation.x += 2;
	worldManager.tryGeneratingNewChunk(chunkLocation, chunksToRerender);
	chunkLocation.x -= 1;

	chunkLocation.y -= 1;
	worldManager.tryGeneratingNewChunk(chunkLocation, chunksToRerender);
	chunkLocation.y += 2;
	worldManager.tryGeneratingNewChunk(chunkLocation, chunksToRerender);
	chunkLocation.y -= 1;

	chunkLocation.z -= 1;
	worldManager.tryGeneratingNewChunk(chunkLocation, chunksToRerender);
	chunkLocation.z += 2;
	worldManager.tryGeneratingNewChunk(chunkLocation, chunksToRerender);
	chunkLocation.z -= 1;

	for (auto chunk : chunksToRerender) {
		if (chunk.x == chunkLocation.x && chunk.y == chunkLocation.y && chunk.z == chunkLocation.z) {
			tryAddChunkToRender(chunk);
		}
	}

	/*static int chunksGenerated = 0;
	static double averageTime = 0.0f;
	auto startTime = std::chrono::high_resolution_clock::now();*/

	std::vector<Vertex> vertices;
	//generateChunkMeshData(worldManager, chunkLocation, vertices);
	binaryGreedyMeshChunk(worldManager, chunkLocation, vertices);

	/*auto endTime = std::chrono::high_resolution_clock::now();
	double chunkGenerationTime = std::chrono::duration<double, std::chrono::microseconds::period>(endTime - startTime).count();
	std::cout << "time in generating chunk is " << chunkGenerationTime << "\n";
	chunksGenerated++;
	averageTime = (averageTime * (chunksGenerated - 1) + chunkGenerationTime) / chunksGenerated;
	std::cout << " average time up to this point is " << averageTime << "\n";*/
	
	if (vertices.size() == 0) {
		return;
	}
	uint32_t memoryBlockPointer = vertexBufferManager.addVerticesToWorld(vulkanCoreInfo, commandPool, vertices);

	renderedChunks.insert(std::make_pair(chunkLocation, memoryBlockPointer));
}