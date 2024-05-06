#pragma once

#include <World/BlockType.hpp>

const int CHUNK_SIZE = 4;

struct Chunk
{
	short* blocks = new short[CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE];
};

int chunkLocationToIndex(const int x, const int y, const int z);
void chunkIndexToLocation(const int index, int& x, int& y, int& z);