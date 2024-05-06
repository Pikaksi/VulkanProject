#pragma once

#include <vector>
#include <unordered_map>

#include "glm/vec3.hpp"

#include "Vertex.hpp"
#include "World/WorldManager.hpp"
#include "VertexCreator.hpp"

class VertexBufferManager;

class ChunkRenderer
{
public:
	std::vector<glm::i32vec3> chunksToRender;

	ChunkRenderer()
	{
		chunksToRender.push_back(glm::i32vec3(0, 0, 0));
		chunksToRender.push_back(glm::i32vec3(1, 0, 0));
		chunksToRender.push_back(glm::i32vec3(0, 0, 1));
		chunksToRender.push_back(glm::i32vec3(1, 0, 1));
		chunksToRender.push_back(glm::i32vec3(0, 1, 0));
		chunksToRender.push_back(glm::i32vec3(1, 1, 0));
		chunksToRender.push_back(glm::i32vec3(0, 1, 1));
		chunksToRender.push_back(glm::i32vec3(-1, 1, -1));
	}
	
	void renderNewChunks(WorldManager& worldManager, VertexBufferManager& vertexBufferManager);
	void renderChunk(glm::i32vec3 chunkLocation, WorldManager& worldManager, VertexBufferManager& vertexBufferManager);
};
#include "VertexBufferManager.hpp"