#pragma once

#include <glm/vec3.hpp>

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

int chunkLocationToIndex(const int x, const int y, const int z);
int chunkLocationToIndex(const int x, const int y);
bool isLocationOutOfChunk(int x, int y, int z);
bool isLocationOutOfChunk(int x, int y);
void chunkIndexToLocation(const int index, int& x, int& y, int& z);

glm::ivec3 worldToChunkLocation(int x, int y, int z);
glm::ivec3 worldToChunkLocation(glm::ivec3 loc);
glm::i32vec3 worldToBlockLocation(glm::i32vec3 loc);

void chunkResize(Chunk& chunk, bool containsDifferentBlocks);
BlockType chunkGetBlockAtLocation(const int x, const int y, const int z, Chunk& chunk);
BlockType chunkGetBlockAtLocation(const glm::i32vec3 loc, Chunk& chunk);
void chunkSetBlock(int x, int y, int z, BlockType blockType, Chunk& chunk);
void chunkSetBlock(int i, BlockType blockType, Chunk& chunk);

int alwaysPosModulo(int value, unsigned int m);
