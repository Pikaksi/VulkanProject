#pragma once

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
};

BlockType chunkGetBlockAtLocation(const int x, const int y, const int z, Chunk* chunk);
int chunkLocationToIndex(const int x, const int y, const int z);
void chunkIndexToLocation(const int index, int& x, int& y, int& z);