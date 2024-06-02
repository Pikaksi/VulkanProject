#pragma once

#include <vector>
#include <unordered_map>
#include <queue>

#include "glm/vec3.hpp"

#include <unordered_set>

#include "Vertex.hpp"
#include "World/WorldManager.hpp"
#include "VertexCreator.hpp"
#include "VulkanRendering/VulkanTypes.hpp"

class VertexBufferManager;

class ChunkRenderer
{
private:
	const int renderDistance = 4;
	std::queue<glm::i32vec3> chunksToRender;
	std::unordered_set<glm::i32vec3> renderedChunks;
	glm::i32vec3 lastChunkRenderingLocation = glm::i32vec3(9999, 9999, 9999);
public:

	ChunkRenderer()
	{

	}

	void tryAddChunksToRender(glm::i32vec3 chunkLocation);
	void addChunksToBeRendered(glm::i32vec3 position);
	void renderNewChunks(VulkanCoreInfo* vulkanCoreInfo, VkCommandPool commandPool, WorldManager& worldManager, VertexBufferManager& vertexBufferManager);
	void renderChunk(VulkanCoreInfo* vulkanCoreInfo, VkCommandPool commandPool, glm::i32vec3 chunkLocation, WorldManager& worldManager, VertexBufferManager& vertexBufferManager);
};
#include "VertexBufferManager.hpp"