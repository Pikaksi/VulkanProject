#pragma once

#include "glm/vec3.hpp"

#include <vector>
#include <unordered_map>
#include <queue>

#include "Rendering/Vertex.hpp"
#include "World/WorldManager.hpp"
#include "VertexCreator.hpp"
#include "VulkanRendering/VulkanTypes.hpp"
#include "Rendering/VertexBufferManager.hpp"

class ChunkRenderer
{
public:
	ChunkRenderer() {}
	void update(
		VulkanCoreInfo& vulkanCoreInfo, 
		VkCommandPool commandPool, 
		WorldManager& worldManager, 
		VertexBufferManager& vertexBufferManager, 
		glm::i32vec3 playerChunkLocation);

private:
	const int renderDistance = 4;
	const int extraRangeToDerenderChunk = 1;
	
	std::queue<glm::i32vec3> chunksToRender;
	std::unordered_map<glm::i32vec3, uint32_t> renderedChunks; // holds GPU memory pointer to delete chunks
	glm::i32vec3 peviousPlayerChunkLocation = glm::i32vec3(9999, 9999, 9999);

	void addChunksToBeRendered(glm::i32vec3 playerChunkLocation, int newChunkMinDistance);
	void derenderChunks(glm::i32vec3 playerChunkLocation, VertexBufferManager& vertexBufferManager);
	void tryAddChunksToRender(glm::i32vec3 chunkLocation);
	void renderNewChunks(
		VulkanCoreInfo& vulkanCoreInfo,
		VkCommandPool commandPool,
		WorldManager& worldManager,
		VertexBufferManager& vertexBufferManager,
		glm::i32vec3 playerChunkLocation);
	void renderChunk(
		VulkanCoreInfo& vulkanCoreInfo,
		VkCommandPool commandPool,
		glm::i32vec3 chunkLocation,
		WorldManager& worldManager,
		VertexBufferManager& vertexBufferManager);
};