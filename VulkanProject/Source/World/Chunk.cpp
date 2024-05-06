#include "Chunk.hpp"

int chunkLocationToIndex(int x, int y, int z)
{
	return x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_SIZE;
}

void chunkIndexToLocation(int index, int& x, int& y, int& z)
{
	x = index % CHUNK_SIZE;
	y = index / CHUNK_SIZE % CHUNK_SIZE;
	z = index / (CHUNK_SIZE * CHUNK_SIZE);
}