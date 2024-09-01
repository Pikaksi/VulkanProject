#include "Chunk.hpp"

#include <iostream>

void Chunk::resize(bool allocateFullSize)
{
	if (allocateFullSize) {
		BlockType originalBlock = blocks[0];
		blocks.resize(CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE);
		containsDifferentBlocks = true;

		for (int zLoop = 0; zLoop < CHUNK_SIZE; zLoop++) {
			for (int yLoop = 0; yLoop < CHUNK_SIZE; yLoop++) {
				for (int xLoop = 0; xLoop < CHUNK_SIZE; xLoop++) {
					blocks[chunkLocationToIndex(xLoop, yLoop, zLoop)] = originalBlock;
				}
			}
		}
	}
	else {
		blocks.resize(1);
		containsDifferentBlocks = false;
	}
}

bool locationOutOfChunk(int x, int y, int z)
{
	return x < 0 || x >= CHUNK_SIZE || y < 0 || y >= CHUNK_SIZE || z < 0 || z >= CHUNK_SIZE;
}

bool locationOutOfChunk(int x, int y)
{
	return x < 0 || x >= CHUNK_SIZE || y < 0 || y >= CHUNK_SIZE;
}

int chunkLocationToIndex(const int x, const int y, const int z)
{
#ifndef NDEBUG
	if (locationOutOfChunk(x, y, z)) {
		throw std::runtime_error("tried to access chunk out of range!");
	}
#endif

	return x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_SIZE;
}

int chunkLocationToIndex(const int x, const int y)
{
#ifndef NDEBUG
	if (locationOutOfChunk(x, y)) {
		throw std::runtime_error("Tried to access blockFaceBitMask out of range!");
	}
#endif

	return x + y * CHUNK_SIZE;
}

void chunkIndexToLocation(const int index, int& x, int& y, int& z)
{
	x = index % CHUNK_SIZE;
	y = index / CHUNK_SIZE % CHUNK_SIZE;
	z = index / (CHUNK_SIZE * CHUNK_SIZE);
}

BlockType chunkGetBlockAtLocation(const int x, const int y, const int z, Chunk* chunk)
{
#ifndef NDEBUG
	if (locationOutOfChunk(x, y, z)) {
		throw std::runtime_error("tried to access chunk out of range!");
	}
#endif

	return chunk->blocks[chunk->containsDifferentBlocks * (x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_SIZE)];
}

BlockType chunkGetBlockAtLocation(const int x, const int y, const int z, Chunk& chunk)
{
#ifndef NDEBUG
	if (locationOutOfChunk(x, y, z)) {
		throw std::runtime_error("tried to access chunk out of range!");
	}
#endif

	return chunk.blocks[chunk.containsDifferentBlocks * (x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_SIZE)];
}

glm::ivec3 getChunkLocation(int x, int y, int z)
{
	return glm::ivec3(floor(x / (float)CHUNK_SIZE), floor(y / (float)CHUNK_SIZE), floor(z / (float)CHUNK_SIZE));
}

glm::ivec3 getChunkLocation(glm::ivec3 blockLocation)
{
	return glm::ivec3(floor(blockLocation.x / (float)CHUNK_SIZE), floor(blockLocation.y / (float)CHUNK_SIZE), floor(blockLocation.z / (float)CHUNK_SIZE));
}

void chunkSetBlock(int x, int y, int z, BlockType blockType, Chunk& chunk)
{
	if (!chunk.containsDifferentBlocks && chunk.blocks[0] != blockType) {
		chunk.resize(true);
	}

	if (chunk.containsDifferentBlocks) {
		//std::cout << "set block " << (int)blockType << " at " << x << " " << y << " " << z << "\n";
		chunk.blocks[chunkLocationToIndex(x, y, z)] = blockType;
	}
}

int negativeModulo(int value, unsigned int m) {
	int mod = value % m;
	if (mod < 0) {
		mod += m;
	}
	return mod;
}