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

bool isLocationOutOfChunk(int x, int y, int z)
{
    return x < 0 || x >= CHUNK_SIZE || y < 0 || y >= CHUNK_SIZE || z < 0 || z >= CHUNK_SIZE;
}

bool isLocationOutOfChunk(int x, int y) { return x < 0 || x >= CHUNK_SIZE || y < 0 || y >= CHUNK_SIZE; }

int chunkLocationToIndex(const int x, const int y, const int z)
{
    assertm(!isLocationOutOfChunk(x, y, z), "tried to access chunk out of range");

    return x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_SIZE;
}

int chunkLocationToIndex(const int x, const int y)
{
    assertm(!isLocationOutOfChunk(x, y), "tried to access 2d chunk out of range");

    return x + y * CHUNK_SIZE;
}

void chunkIndexToLocation(const int index, int& x, int& y, int& z)
{
    assertm(index >= 0 && index < (CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE), "Index is not valid");

    x = index % CHUNK_SIZE;
    y = index / CHUNK_SIZE % CHUNK_SIZE;
    z = index / (CHUNK_SIZE * CHUNK_SIZE);
}

BlockType chunkGetBlockAtLocation(const glm::i32vec3 loc, Chunk& chunk)
{
    assertm(!isLocationOutOfChunk(loc.x, loc.y, loc.z), "tried to access chunk out of range");

    return chunk.blocks[chunk.containsDifferentBlocks * (loc.x + loc.y * CHUNK_SIZE + loc.z * CHUNK_SIZE * CHUNK_SIZE)];
}

BlockType chunkGetBlockAtLocation(const int x, const int y, const int z, Chunk& chunk)
{
    assertm(!isLocationOutOfChunk(x, y, z), "tried to access chunk out of range");

    return chunk.blocks[chunk.containsDifferentBlocks * (x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_SIZE)];
}

glm::ivec3 worldToChunkLocation(int x, int y, int z)
{
    return glm::ivec3(
        std::floor(x / (float)CHUNK_SIZE), std::floor(y / (float)CHUNK_SIZE), std::floor(z / (float)CHUNK_SIZE));
}

glm::ivec3 worldToChunkLocation(glm::ivec3 loc)
{
    return glm::ivec3(std::floor(loc.x / (float)CHUNK_SIZE),
                      std::floor(loc.y / (float)CHUNK_SIZE),
                      std::floor(loc.z / (float)CHUNK_SIZE));
}

glm::i32vec3 worldToBlockLocation(glm::i32vec3 loc)
{
    return glm::i32vec3{
        alwaysPosModulo(loc.x, CHUNK_SIZE), alwaysPosModulo(loc.y, CHUNK_SIZE), alwaysPosModulo(loc.z, CHUNK_SIZE)};
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

void chunkSetBlock(int i, BlockType blockType, Chunk& chunk)
{
    if (!chunk.containsDifferentBlocks && chunk.blocks[0] != blockType) {
        chunkResize(chunk, true);
    }

    if (chunk.containsDifferentBlocks) {
        chunk.blocks[i] = blockType;
    }
}
