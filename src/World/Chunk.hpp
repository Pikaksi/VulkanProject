#pragma once

#include <glm/vec3.hpp>

#include <iostream>
#include <vector>

#include "BlockType.hpp"

const int CHUNK_SIZE = 32;


struct Chunk
{
	bool containsDifferentBlocks;
	std::vector<BlockType> blocks;

	Chunk(bool containsDifferentBlocks = true)
	{
		this->containsDifferentBlocks = containsDifferentBlocks;
		if (containsDifferentBlocks) {
			blocks.resize(CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE);
		}
		else {
			blocks.resize(1);
		}
	}
	void resize(bool containsDifferentBlocks);
};

int chunkLocationToIndex(const int x, const int y, const int z);
bool locationOutOfChunk(int x, int y, int z);
bool locationOutOfChunk(int x, int y);
int chunkLocationToIndex(const int x, const int y);
void chunkIndexToLocation(const int index, int& x, int& y, int& z);
BlockType chunkGetBlockAtLocation(const int x, const int y, const int z, Chunk* chunk);
BlockType chunkGetBlockAtLocation(const int x, const int y, const int z, Chunk& chunk);
glm::ivec3 getChunkLocation(int x, int y, int z);
glm::ivec3 getChunkLocation(glm::ivec3 blockLocation);
void chunkSetBlock(int x, int y, int z, BlockType blockType, Chunk& chunk);
int negativeModulo(int value, unsigned int m);