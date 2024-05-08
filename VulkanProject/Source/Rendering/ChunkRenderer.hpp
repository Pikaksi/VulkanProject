#pragma once

#include <vector>
#include <unordered_map>
#include <queue>

#include "glm/vec3.hpp"

#include <unordered_set>

#include "Vertex.hpp"
#include "World/WorldManager.hpp"
#include "VertexCreator.hpp"

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
		/*
		chunksToRender.push_back(glm::i32vec3(0, 0, 0));
		chunksToRender.push_back(glm::i32vec3(1, 0, 0));
		chunksToRender.push_back(glm::i32vec3(0, 0, 1));
		chunksToRender.push_back(glm::i32vec3(1, 0, 1));
		chunksToRender.push_back(glm::i32vec3(0, 1, 0));
		chunksToRender.push_back(glm::i32vec3(1, 1, 0));
		chunksToRender.push_back(glm::i32vec3(0, 1, 1));
		chunksToRender.push_back(glm::i32vec3(-1, 0, 0));
		chunksToRender.push_back(glm::i32vec3(-1, 1, -1));
		*/
	}

	void tryAddChunksToRender(glm::i32vec3 chunkLocation);
	void addChunksToBeRendered(glm::i32vec3 position);
	void renderNewChunks(WorldManager& worldManager, VertexBufferManager& vertexBufferManager);
	void renderChunk(glm::i32vec3 chunkLocation, WorldManager& worldManager, VertexBufferManager& vertexBufferManager);
};
#include "VertexBufferManager.hpp"