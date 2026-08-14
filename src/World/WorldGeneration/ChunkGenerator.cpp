#include <iostream>
#include <random>

#include "ChunkGenerator.hpp"
#include "ChunkRenderer.hpp"
#include "FastNoise/FastNoise.h"

const int grassDirtLayerHeight = 3;
const float noiseAirMinValue = 0.0f;

const int treetopNoiseMapRadius = 10;
const int treetopLogMaxRadius = 7;
const int treeTrunkExtraHeight = 8;
const int treeStructureHeightOffset = -3;
const float treeNoiseFrequency = 1;
const int treeNoiseWidth = treetopNoiseMapRadius * 2 + 1;
const int treeNoiseHeight = treetopNoiseMapRadius * 2 + 1 + treeTrunkExtraHeight;

int groundNoiseChunkLocationToIndex(const int x, const int y, const int z)
{
#ifndef NDEBUG
    if (x < 0 || x >= CHUNK_SIZE || y < 0 || y >= (CHUNK_SIZE + grassDirtLayerHeight) || z < 0 || z >= CHUNK_SIZE) {
        throw std::runtime_error("tried to access noise chunk out of range!");
    }
#endif

    return z + y * CHUNK_SIZE + x * CHUNK_SIZE * (CHUNK_SIZE + grassDirtLayerHeight);
}

int treeNoiseChunkLocationToIndex(const int x, const int y, const int z)
{
#ifndef NDEBUG
    if (x < 0 || x >= treeNoiseWidth || y < 0 || y >= treeNoiseHeight || z < 0 || z >= treeNoiseWidth) {
        throw std::runtime_error("tried to access noise chunk out of range!");
    }
#endif

    return z + y * treeNoiseWidth + x * treeNoiseHeight * treeNoiseWidth;
}

// Does not care if x, y and z are outside of chunk.
void placeStructureBlock(int x,
                         int y,
                         int z,
                         glm::ivec3 chunkLocationOffset,
                         BlockType block,
                         WorldManager* worldManager,
                         ChunkRenderer& chunkRenderer)
{
    glm::ivec3 chunkLocation = getChunkLocation(x, y, z) + chunkLocationOffset;
    glm::ivec3 blockLocation = glm::ivec3(alwaysPosModulo(x, 32), alwaysPosModulo(y, 32), alwaysPosModulo(z, 32));

    if (worldManager->chunks.contains(chunkLocation)) {
        Chunk& chunk = worldManager->chunks.at(chunkLocation);
        BlockType blockToReplace = chunkGetBlockAtLocation(
            blockLocation.x, blockLocation.y, blockLocation.z, worldManager->chunks.at(chunkLocation));

        if (blockToReplace == BlockType::air) {
            chunkSetBlock(
                blockLocation.x, blockLocation.y, blockLocation.z, block, worldManager->chunks.at(chunkLocation));
            chunkRenderer.rerenderChunkAgain(chunkLocation);
        }
        return;
    }

    if (!worldManager->ungeneratedStructures.contains(chunkLocation)) {
        worldManager->ungeneratedStructures.insert(std::make_pair(chunkLocation, Chunk(false)));
    }
    chunkSetBlock(blockLocation.x,
                  blockLocation.y,
                  blockLocation.z,
                  block,
                  worldManager->ungeneratedStructures.at(chunkLocation));
}

// X, y and z might not be in chunk.
void generateTree(int x,
                  int y,
                  int z,
                  glm::i32vec3 chunkLocation,
                  WorldManager* worldManager,
                  std::vector<glm::ivec3>& chunksToRerender,
                  ChunkRenderer& chunkRenderer)
{
    // for the tool:
    // log:
    // GwAZABsAJwABAAAABQAAAAAAAAAAAAAAAAAAAAAAAAAAAABmZqY/ARkABAAAAAAAKVwPPgAAAAAAAAAAAAAAAIXrkUAAAAAAAAAAAADNzEy/ARkAGQAbABkAHgAdABkAEwCamZk+GgABEQACAAAAAADgQBAAAACIQR8AFgABAAAACwADAAAAAgAAAAMAAAAEAAAAAAAAAD8BFAD//wYAAAAAAD8AAAAAPwAAAAA/AAAAAD8BFwAAAIC/AACAPz0KF0BSuB5AEwAAAKBABgAAj8J1PACamZk+AAAAAAAA4XoUPwAAAAAAAAAAgD8AAACAvwEbABkAHgAdABsABQAAAAAAAAAAAAAAAAAAAAAAAAAAAABmZuY+AAAAgD8AAAAAPwAAAAC/AAAAgMAAAACAvwD0/VS8ARsAGQAeAB0AGwAFAAAAAAAAAAAAAAAAAAAAAAAAAAAAAM3MDD8AAACAPwAAAAA/AAAAAL8AAACAQA==
    // leaf:
    // GQAbABkAHgAdABkAEwCamZk+GgABEQACAAAAAADgQBAAAACIQR8AFgABAAAACwADAAAAAgAAAAMAAAAEAAAAAAAAAD8BFAD//wAAAAAAAD8AAAAAPwAAAAA/AAAAAD8BFwAAAIC/AACAPz0KF0BSuB5AEwAAAKBABgAAj8J1PACamZk+AAAAAAAA4XoUPwAAAAAAAAAAgD8AAACAvwEbABkAHgAdABsABQAAAAAAAAAAAAAAAAAAAAAAAAAAAABmZuY+AAAAgD8AAAAAPwAAAAC/AAAAgMAAAACAvwC4HoW9

    // The string is generated with the FastNoise2 noisetool.
    // static FastNoise::SmartNode<> treeNoiseGenerator =
    // FastNoise::NewFromEncodedNodeTree("GwAZAB4AHQAZABMAmpmZPhoAAREAAgAAAAAA4EAQAAAAiEEfABYAAQAAAAsAAwAAAAIAAAADAAAABAAAAAAAAAA/ARQA//8AAAAAAAA/AAAAAD8AAAAAPwAAAAA/ARcAAACAvwAAgD89ChdAUrgeQBMAAACgQAYAAI/CdTwAmpmZPgAAAAAAAOF6FD8AAAAAAAAAAIA/AAAAgL8BGwAbABkAHgAdABsABQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAxCOw+AAAAgD8AAAAAPwAAAAC/AAAAgL8AAACAQAAAAIC/");
    // static FastNoise::SmartNode<> logExtraNoiseGenerator =
    // FastNoise::NewFromEncodedNodeTree("GwAZAB4AHQAbAAUAAAAAAAAAAAAAAAAAAAAAAAAAAAAAzcwMPwAAAIA/AAAAAD8AAAAAvwAAAIBA");
    // static FastNoise::SmartNode<> treeTrunkNoiseGenerator =
    // FastNoise::NewFromEncodedNodeTree("GQAbAB0AGQAEAAAAAAD2KJw/AAAAAAAAAAAAAAAACtcjQAAAAAAAAAAAAM3MTL8AAAAAAAEcAAEZAAgAAFyPgj8AAABAQAEZABsAJwABAAAABQAAAAAAAAAAAAAAAAAAAAAAAAAAAABmZqY/ARkABAAAAAAAaJFtPQAAAAAAAAAAAAAAAOxRKEEAAAAAAAAAAADNzEy/");

    static FastNoise::SmartNode<> treeNoiseGenerator = FastNoise::NewFromEncodedNodeTree("FQkFBANI4ZHCBA==");
    static FastNoise::SmartNode<> logExtraNoiseGenerator =
        FastNoise::NewFromEncodedNodeTree("GwkVCRUJFwkE@BgD8EAv8@AQCFwkEE@AgD8EAv8CAAwDObTAwAQCBAgAAIA/CwAAoEAM");
    static FastNoise::SmartNode<> treeTrunkNoiseGenerator =
        FastNoise::NewFromEncodedNodeTree("GwkVCRUJFwkE@BgD8EAv8@AQCFwkEE@AgD8EAv8CAAwDObTAwAQCBAgAAIA/CwAAoEAM");

    std::vector<float> treeNoiseOutput(treeNoiseWidth * treeNoiseHeight * treeNoiseWidth);
    std::vector<float> logExtraNoiseOutput(treeNoiseWidth * treeNoiseHeight * treeNoiseWidth);
    std::vector<float> treeTrunkNoiseOutput(treeNoiseWidth * treeNoiseHeight * treeNoiseWidth);

    int randomTreeSeed = rand();
    treeNoiseGenerator->GenUniformGrid3D(treeNoiseOutput.data(),
                                         -treetopNoiseMapRadius,
                                         -treetopNoiseMapRadius - treeTrunkExtraHeight,
                                         -treetopNoiseMapRadius,
                                         treeNoiseWidth,
                                         treeNoiseHeight,
                                         treeNoiseWidth,
                                         treeNoiseFrequency,
                                         treeNoiseFrequency,
                                         treeNoiseFrequency,
                                         randomTreeSeed);

    logExtraNoiseGenerator->GenUniformGrid3D(logExtraNoiseOutput.data(),
                                             -treetopNoiseMapRadius,
                                             -treetopNoiseMapRadius - treeTrunkExtraHeight,
                                             -treetopNoiseMapRadius,
                                             treeNoiseWidth,
                                             treeNoiseHeight,
                                             treeNoiseWidth,
                                             treeNoiseFrequency,
                                             treeNoiseFrequency,
                                             treeNoiseFrequency,
                                             randomTreeSeed);

    treeTrunkNoiseGenerator->GenUniformGrid3D(treeTrunkNoiseOutput.data(),
                                              -treetopNoiseMapRadius,
                                              -treetopNoiseMapRadius - treeTrunkExtraHeight,
                                              -treetopNoiseMapRadius,
                                              treeNoiseWidth,
                                              treeNoiseHeight,
                                              treeNoiseWidth,
                                              treeNoiseFrequency,
                                              treeNoiseFrequency,
                                              treeNoiseFrequency,
                                              randomTreeSeed);

    for (int xOffset = 0; xOffset < treeNoiseWidth; xOffset++) {
        for (int yOffset = 0; yOffset < treeNoiseHeight; yOffset++) {
            for (int zOffset = 0; zOffset < treeNoiseWidth; zOffset++) {
                float treeNoiseValue = treeNoiseOutput[treeNoiseChunkLocationToIndex(xOffset, yOffset, zOffset)];
                float logExtraNoiseValue =
                    logExtraNoiseOutput[treeNoiseChunkLocationToIndex(xOffset, yOffset, zOffset)];
                float treeTrunkNoiseValue =
                    treeTrunkNoiseOutput[treeNoiseChunkLocationToIndex(xOffset, yOffset, zOffset)];

                if (treeNoiseValue + logExtraNoiseValue < -3.0f ||
                    treeTrunkNoiseValue < -3.0f) { // value is adjusted by testing the tree generator in FastNoise2
                                                   // noise tool by substracting that amount from the
                    placeStructureBlock(x + xOffset - treetopNoiseMapRadius,
                                        y + yOffset + treeStructureHeightOffset,
                                        z + zOffset - treetopNoiseMapRadius,
                                        chunkLocation,
                                        BlockType::oakLog,
                                        worldManager,
                                        chunkRenderer);
                }
                else if (treeNoiseValue < 0.12f) {
                    placeStructureBlock(x + xOffset - treetopNoiseMapRadius,
                                        y + yOffset + treeStructureHeightOffset,
                                        z + zOffset - treetopNoiseMapRadius,
                                        chunkLocation,
                                        BlockType::oakLeaf,
                                        worldManager,
                                        chunkRenderer);
                }
            }
        }
    }

    glm::ivec3 blockCornerNegative =
        chunkLocation * CHUNK_SIZE +
        glm::ivec3(x - treetopNoiseMapRadius, y + treeStructureHeightOffset, z - treetopNoiseMapRadius) -
        glm::ivec3(32, 32, 32);
    glm::ivec3 blockCornerPositive = chunkLocation * CHUNK_SIZE +
                                     glm::ivec3(x + treetopNoiseMapRadius,
                                                y + treeNoiseHeight + treeStructureHeightOffset,
                                                z + treetopNoiseMapRadius) +
                                     glm::ivec3(32, 32, 32);
    glm::ivec3 chunkCornerNegative = getChunkLocation(blockCornerNegative);
    glm::ivec3 chunkCornerPositive = getChunkLocation(blockCornerPositive);

    for (int chunkX = chunkCornerNegative.x; chunkX <= chunkCornerPositive.x; chunkX++) {
        for (int chunkY = chunkCornerNegative.y; chunkY <= chunkCornerPositive.y; chunkY++) {
            for (int chunkZ = chunkCornerNegative.z; chunkZ <= chunkCornerPositive.z; chunkZ++) {
                chunksToRerender.push_back(glm::i32vec3(chunkX, chunkY, chunkZ));
            }
        }
    }
}

float randomFloat01() { return static_cast<float>(rand()) / static_cast<float>(RAND_MAX); }

void generateChunk(glm::i32vec3 chunkLocation,
                   WorldManager* worldManager,
                   std::vector<glm::ivec3>& chunksToRenderAgain,
                   ChunkRenderer& chunkRenderer)
{
    // The string is generated with the FastNoise2 noisetool.
    static FastNoise::SmartNode<> groundGenerator = FastNoise::NewFromEncodedNodeTree(
        "E@BBZEG@BD8JFgIECArXozsECiQIw/UoPwkuAAE@BJDQAF@BC@AIEAJBw@ABZEED0KV78YZmZmPwQDmpkZPwsAAIA/HAMAAHBCBA==");

    std::vector<float> noiseOutput(CHUNK_SIZE * (CHUNK_SIZE + grassDirtLayerHeight) * CHUNK_SIZE);

    groundGenerator->GenUniformGrid3D(noiseOutput.data(),
                                      chunkLocation.z * CHUNK_SIZE,
                                      chunkLocation.y * CHUNK_SIZE,
                                      chunkLocation.x * CHUNK_SIZE,
                                      CHUNK_SIZE,
                                      CHUNK_SIZE + grassDirtLayerHeight,
                                      CHUNK_SIZE,
                                      frequency,
                                      frequency,
                                      frequency,
                                      1337);

    if (worldManager->ungeneratedStructures.contains(chunkLocation)) {
        worldManager->chunks.insert(
            std::make_pair(chunkLocation, worldManager->ungeneratedStructures.at(chunkLocation)));
        worldManager->ungeneratedStructures.erase(chunkLocation);
    }
    else {
        worldManager->chunks.insert(std::make_pair(chunkLocation, Chunk(true)));
    }

    Chunk& chunk = worldManager->chunks.at(chunkLocation);

    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_SIZE; y++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {

                float noise = noiseOutput[groundNoiseChunkLocationToIndex(x, y, z)];

                if (noise >= noiseAirMinValue) {
                    continue;
                }

                bool isInDirtLayer = false;
                for (int yExtra = 1; yExtra < grassDirtLayerHeight + 1; yExtra++) {
                    if (noiseOutput[groundNoiseChunkLocationToIndex(x, y + yExtra, z)] >= noiseAirMinValue) {
                        isInDirtLayer = true;

                        bool hasAirOnTop = yExtra == 1;
                        if (hasAirOnTop) {
                            worldManager->chunks.at(chunkLocation).blocks[chunkLocationToIndex(x, y, z)] =
                                BlockType::grass;

                            if (randomFloat01() < 0.3f) {
                                placeStructureBlock(
                                    x, y + 1, z, chunkLocation, BlockType::grassPlant, worldManager, chunkRenderer);
                            }
                            if (randomFloat01() < 0.002f) {
                                generateTree(x, y + 1, z, chunkLocation, worldManager, chunksToRenderAgain, chunkRenderer);
                            }
                        }
                        else {
                            chunk.blocks[chunkLocationToIndex(x, y, z)] = BlockType::dirt;
                        }

                        break;
                    }
                }
                if (isInDirtLayer) {
                    continue;
                }

                chunk.blocks[chunkLocationToIndex(x, y, z)] = BlockType::stone;
            }
        }
    }

    bool containsDifferentBlocks = false;
    BlockType previousBlockType = chunk.blocks[0];

    for (int i = 1; i < CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE; i++) {
        if (previousBlockType != chunk.blocks[i]) {
            containsDifferentBlocks = true;
            break;
        }
    }
    if (!containsDifferentBlocks) {
        chunkResize(chunk, false); // contains only one element in block array to reduce space by CHUNK_SIZE * CHUNK_SIZE *
                             // CHUNK_SIZE amount.
    }
}
