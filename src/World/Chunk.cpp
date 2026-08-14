#include "Chunk.hpp"
#include "assertm.hpp"

#include <cmath>
#include <iostream>


int alwaysPosModulo(int value, unsigned int m)
{
    int mod = value % m;
    if (mod < 0) {
        mod += m;
    }
    return mod;
}

void chunkResize(Chunk& chunk, bool allocateFullSize)
{
    if (allocateFullSize) {
        BlockType originalBlock = chunk.blocks[0];
        chunk.blocks.resize(CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE);
        chunk.containsDifferentBlocks = true;

        for (int zLoop = 0; zLoop < CHUNK_SIZE; zLoop++) {
            for (int yLoop = 0; yLoop < CHUNK_SIZE; yLoop++) {
                for (int xLoop = 0; xLoop < CHUNK_SIZE; xLoop++) {
                    chunk.blocks[chunkLocationToIndex(xLoop, yLoop, zLoop)] = originalBlock;
                }
            }
        }
    }
    else {
        chunk.blocks.resize(1);
        chunk.blocks.shrink_to_fit();
        chunk.containsDifferentBlocks = false;
    }
}

bool locationOutOfChunk(int x, int y, int z)
{
    return x < 0 || x >= CHUNK_SIZE || y < 0 || y >= CHUNK_SIZE || z < 0 || z >= CHUNK_SIZE;
}

bool locationOutOfChunk(int x, int y) { return x < 0 || x >= CHUNK_SIZE || y < 0 || y >= CHUNK_SIZE; }

int chunkLocationToIndex(const int x, const int y, const int z)
{
    assertm(!locationOutOfChunk(x, y, z), "tried to access chunk out of range");

    return x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_SIZE;
}

int chunkLocationToIndex(const int x, const int y)
{
    assertm(!locationOutOfChunk(x, y), "tried to access 2d chunk out of range");

    return x + y * CHUNK_SIZE;
}

void chunkIndexToLocation(const int index, int& x, int& y, int& z)
{
    assertm(index >= 0 && index < (CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE), "Index is not valid");

    x = index % CHUNK_SIZE;
    y = index / CHUNK_SIZE % CHUNK_SIZE;
    z = index / (CHUNK_SIZE * CHUNK_SIZE);
}

BlockType chunkGetBlockAtLocation(const int x, const int y, const int z, Chunk* chunk)
{
    assertm(!locationOutOfChunk(x, y, z), "tried to access chunk out of range");

    return chunk->blocks[chunk->containsDifferentBlocks * (x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_SIZE)];
}

BlockType chunkGetBlockAtLocation(const int x, const int y, const int z, Chunk& chunk)
{
    assertm(!locationOutOfChunk(x, y, z), "tried to access chunk out of range");

    return chunk.blocks[chunk.containsDifferentBlocks * (x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_SIZE)];
}

glm::ivec3 getChunkLocation(int x, int y, int z)
{
    return glm::ivec3(
        std::floor(x / (float)CHUNK_SIZE), std::floor(y / (float)CHUNK_SIZE), std::floor(z / (float)CHUNK_SIZE));
}

glm::ivec3 getChunkLocation(glm::ivec3 blockLocation)
{
    return glm::ivec3(std::floor(blockLocation.x / (float)CHUNK_SIZE),
                      std::floor(blockLocation.y / (float)CHUNK_SIZE),
                      std::floor(blockLocation.z / (float)CHUNK_SIZE));
}

void chunkSetBlock(int x, int y, int z, BlockType blockType, Chunk& chunk)
{
    if (!chunk.containsDifferentBlocks && chunk.blocks[0] != blockType) {
        chunkResize(chunk, true);
    }

    if (chunk.containsDifferentBlocks) {
        chunk.blocks[chunkLocationToIndex(x, y, z)] = blockType;
    }
}

