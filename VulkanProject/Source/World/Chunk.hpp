#pragma once

#include <World/BlockType.hpp>

const int CHUNK_SIZE = 32;

struct Chunk
{
	BlockType* blocks = new BlockType[CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE];
};

int chunkLocationToIndex(const int x, const int y, const int z);
void chunkIndexToLocation(const int index, int& x, int& y, int& z);