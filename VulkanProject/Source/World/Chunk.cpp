#include "Chunk.hpp"

#include <iostream>

int chunkLocationToIndex(const int x, const int y, const int z)
{
#ifndef NDEBUG
	if (x < 0 || x >= CHUNK_SIZE || y < 0 || y >= CHUNK_SIZE || z < 0 || z >= CHUNK_SIZE) {
		throw std::runtime_error("tried to access chunk out of range!");
	}
#endif

	return x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_SIZE;
}

void chunkIndexToLocation(const int index, int& x, int& y, int& z)
{
	x = index % CHUNK_SIZE;
	y = index / CHUNK_SIZE % CHUNK_SIZE;
	z = index / (CHUNK_SIZE * CHUNK_SIZE);
}