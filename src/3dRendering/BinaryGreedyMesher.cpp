#include "BinaryGreedyMesher.hpp"

#include <chrono>
#include <numbers>
#include <bitset>
#include <algorithm>

#include "BlockTexCoordinateLookup.hpp"
#include "BlockType.hpp"
#include "DebugMenu.hpp"
#include "World/BlockDataLookup.hpp"
#include "Rendering/TextureCreator.hpp"
#include "World/Chunk.hpp"
#include "assertm.hpp"

typedef uint64_t BlockBitMask;

const uint64_t debruijn64 = 0x03f79d71b4cb0a89;
const int index64[64] = {0,  1,  48, 2,  57, 49, 28, 3,  61, 58, 50, 42, 38, 29, 17, 4,  62, 55, 59, 36, 53, 51,
                         43, 22, 45, 39, 33, 30, 24, 18, 12, 5,  63, 47, 56, 27, 60, 41, 37, 16, 54, 35, 52, 21,
                         44, 32, 23, 11, 46, 26, 40, 15, 34, 20, 31, 10, 25, 14, 19, 9,  13, 8,  7,  6};

// https://www.chessprogramming.org/index.php?title=BitScan
int LovestSignificantBitIndex(uint64_t bitboard) { return index64[((bitboard & (0 - bitboard)) * debruijn64) >> 58]; }

void printBitMask(BlockBitMask blockBitMask)
{
    for (int i = CHUNK_SIZE + 1; i >= 0; i--) {
        std::cout << (uint64_t)((blockBitMask & (1 << i)) >> i);
    }
}

void printChunkLayerX(Chunk& chunk, int x)
{
    for (int y = 0; y < CHUNK_SIZE; y++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            std::cout << (int)chunkGetBlockAtLocation(x, y, z, &chunk);
        }
        std::cout << "\n";
    }
}

constexpr BlockBitMask chunkOuterBitsOff()
{
    return ~(((BlockBitMask)0b1) | (((BlockBitMask)0b1) << (CHUNK_SIZE + 1)));
}

BlockBitMask getBlockFacesLeftShift(BlockBitMask blockBitMask)
{
    return (((~blockBitMask) >> 1) & blockBitMask) & chunkOuterBitsOff();
}

BlockBitMask getBlockFacesRightShift(BlockBitMask blockBitMask)
{
    return (((~blockBitMask) << 1) & blockBitMask) & chunkOuterBitsOff();
}

int blockArrayLocToIndex(int x, int y, int z)
{
    return x + y * (CHUNK_SIZE + 2) + z * (CHUNK_SIZE + 2) * (CHUNK_SIZE + 2);
}

// ----------- NEW MESHER-----------

// clang-format off

void addMeshFacePosZ(int x, int y, int z, int width, int height, BlockType block, std::vector<MeshFace>& faces)
{
    faces.push_back(MeshFace{
        .location = {
            glm::vec3{x, y, z + 1},
            glm::vec3{x, y + height, z + 1},
            glm::vec3{x + width, y + height, z + 1},
            glm::vec3{x + width, y, z + 1},
        },
        .uv = {
            {0.0f, (float)height},
            {0.0f, 0.0f},
            {(float)width, 0.0f},
            {(float)width, (float)height},
        },
        .textureLayer = (uint32_t)getBlockTextureLayer(block, 4)
    });
}

void addMeshFaceNegZ(int x, int y, int z, int width, int height, BlockType block, std::vector<MeshFace>& faces)
{
    faces.push_back(MeshFace{
        .location = {
            glm::vec3{x, y, z},
            glm::vec3{x + width, y, z},
            glm::vec3{x + width, y + height, z},
            glm::vec3{x, y + height, z},
        },
        .uv = {
            {(float)width, (float)height},
            {0.0f, (float)height},
            {0.0f, 0.0f},
            {(float)width, 0.0f},
        },
        .textureLayer = (uint32_t)getBlockTextureLayer(block, 5)
 
    });
}

void addMeshFacePosX(int x, int y, int z, int width, int height, BlockType block, std::vector<MeshFace>& faces)
{
    faces.push_back(MeshFace{
        .location = {
            glm::vec3{x + 1, y, z},
            glm::vec3{x + 1, y, z + height},
            glm::vec3{x + 1, y + width, z + height},
            glm::vec3{x + 1, y + width, z},
        },
        .uv = {
            {(float)height, (float)width},
            {0.0f, (float)width},
            {0.0f, 0.0f},
            {(float)height, 0.0f},
        },
        .textureLayer = (uint32_t)getBlockTextureLayer(block, 0)
    });
}

void addMeshFaceNegX(int x, int y, int z, int width, int height, BlockType block, std::vector<MeshFace>& faces)
{
    faces.push_back(MeshFace{
        .location = {
            glm::vec3{x, y, z},
            glm::vec3{x, y + width, z},
            glm::vec3{x, y + width, z + height},
            glm::vec3{x, y, z + height},
        },
        .uv = {
            {(float)height, (float)width},
            {(float)height, 0.0f},
            {0.0f, 0.0f},
            {0.0f, (float)width},
        },
        .textureLayer = (uint32_t)getBlockTextureLayer(block, 1)
    });
}

void addMeshFacePosY(int x, int y, int z, int width, int height, BlockType block, std::vector<MeshFace>& faces)
{
    faces.push_back(MeshFace{
        .location = {
            glm::vec3{x, y + 1, z},
            glm::vec3{x + width, y + 1, z},
            glm::vec3{x + width, y + 1, z + height},
            glm::vec3{x, y + 1, z + height},
        },
        .uv = {
            {(float)height, (float)width},
            {(float)height, 0.0f},
            {0.0f, 0.0f},
            {0.0f, (float)width},
        },
        .textureLayer = (uint32_t)getBlockTextureLayer(block, 2)
    });
}

void addMeshFaceNegY(int x, int y, int z, int width, int height, BlockType block, std::vector<MeshFace>& faces)
{
    faces.push_back(MeshFace{
        .location = {
            glm::vec3{x, y, z},
            glm::vec3{x, y, z + height},
            glm::vec3{x + width, y, z + height},
            glm::vec3{x + width, y, z},
        },
        .uv = {
            {(float)width, (float)height},
            {(float)width, 0.0f},
            {0.0f, 0.0f},
            {0.0f, (float)height},
        },
        .textureLayer = (uint32_t)getBlockTextureLayer(block, 3)
    });
}

// clang-format on

void coordinateSwapX(int& x, int& y, int& z)
{
    int tmp = z;
    z = y;
    y = x;
    x = tmp;
}

void coordinateSwapY(int& x, int& y, int& z)
{
    int tmp = y;
    y = z;
    z = tmp;
}

void coordinateSwapZ(int& x, int& y, int& z) {}

using Fn1 = void (*)(int&, int&, int&);
using Fn2 = void (*)(int, int, int, int, int, BlockType, std::vector<MeshFace>&);

template <Fn1 coordinateSwap> int blockArrayLookupTemplate(int x, int y, int z)
{
    coordinateSwap(x, y, z);
    return blockArrayLocToIndex(x, y, z);
}

// Coordinate function is used to access blocks and can be changed to effectively rotate the array.
template <Fn1 coordinateSwap, Fn2 addMeshFace>
void mergeFaces(BlockBitMask* hasFace,
                std::vector<MeshFace>& faces,
                std::array<BlockType, (CHUNK_SIZE + 2) * (CHUNK_SIZE + 2) * (CHUNK_SIZE + 2)>& blocks)
{
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_SIZE; y++) {
            BlockBitMask blockBitMask = hasFace[x + y * CHUNK_SIZE];
            // printBitMask(blockBitMask);
            // std::cout << std::endl;

            while (blockBitMask != 0) {
                int width = 1;
                int height = 1;
                BlockBitMask startingBlock = blockBitMask & (0 - blockBitMask); // get least significant bit
                hasFace[x + y * CHUNK_SIZE] ^= startingBlock; // turn off used bit mask  TODO: might be unnecessary
                int z = LovestSignificantBitIndex(startingBlock);

                BlockType originalBlockType = blocks[blockArrayLookupTemplate<coordinateSwap>(x + 1, y + 1, z + 1)];
                /*std::cout << "original block = " << (int)originalBlockType << " x,y = " << x + 1 << " " << y + 1 <<
                std::endl;
                {
                    int x2 = x + 1, y2 = y + 1, z2 = z + 1;
                    std::cout << "transformed coordinates = " << x2 << " " << y2 << " " << z2 << std::endl;
                    coordinateSwap(x2, y2, z2);
                    std::cout << "transformed coordinates = " << x2 << " " << y2 << " " << z2 << std::endl;
                }*/

                // Expand sideways as much as possible
                for (int xExpansion = 1; xExpansion < CHUNK_SIZE - x; xExpansion++) {

                    BlockBitMask neighborBitMask = blockBitMask & hasFace[x + xExpansion + y * CHUNK_SIZE];
                    BlockType neightborBlock =
                        blocks[blockArrayLookupTemplate<coordinateSwap>(x + xExpansion + 1, y + 1, z + 1)];

                    if (neighborBitMask == 0 || originalBlockType != neightborBlock) {
                        break;
                    }
                    hasFace[x + xExpansion + y * CHUNK_SIZE] ^= startingBlock;
                    width++;
                }

                // Expand forward
                for (int yExpansion = 1; yExpansion < CHUNK_SIZE - y; yExpansion++) {
                    bool cantExpand = false;

                    for (int xExpansion = 0; xExpansion < width; xExpansion++) {
                        BlockBitMask neighborBitMask =
                            blockBitMask & hasFace[x + xExpansion + (y + yExpansion) * CHUNK_SIZE];
                        BlockType neightborBlock = blocks[blockArrayLookupTemplate<coordinateSwap>(
                            x + xExpansion + 1, y + yExpansion + 1, z + 1)];

                        if (neighborBitMask == 0 || originalBlockType != neightborBlock) {
                            cantExpand = true;
                            break;
                        }
                    }
                    if (cantExpand) {
                        break;
                    }
                    // Turn off blocks that can be merged.
                    for (int xExpansion = 0; xExpansion < width; xExpansion++) {
                        hasFace[x + xExpansion + (y + yExpansion) * CHUNK_SIZE] ^= startingBlock;
                    }
                    height++;
                }

                int x2 = x, y2 = y, z2 = z;
                coordinateSwap(x2, y2, z2);
                addMeshFace(x2, y2, z2, width, height, originalBlockType, faces);

                blockBitMask ^= startingBlock; // turn off the bit that was used
            }
        }
    }
}

void meshCustomBlock(int x, int y, int z, BlockType blockType, std::vector<MeshFace>& faces)
{
    uint32_t textureArrayIndex = (uint32_t)getBlockTextureLayer(blockType, 0);

    glm::vec3 blockLocation = glm::ivec3(x, y, z);
    std::vector<glm::vec3> vertexOffsets = blockCustomRenderVertexOffsets.at(blockType);

    for (size_t i = 0; i < vertexOffsets.size(); i += 4) {
        // clang-format off
        faces.push_back(MeshFace{
            .location = {
                blockLocation + vertexOffsets[i],
                blockLocation + vertexOffsets[i + 1],
                blockLocation + vertexOffsets[i + 2],
                blockLocation + vertexOffsets[i + 3],
            },
            .uv = {
                glm::vec2(0.0f, 1.0f),
                glm::vec2(1.0f, 1.0f),
                glm::vec2(1.0f, 0.0f),
                glm::vec2(0.0f, 0.0f),
            },
            .textureLayer = textureArrayIndex
        });
        // clang-format on
    }
}

void binaryGreedyMeshChunk3(std::array<BlockType, (CHUNK_SIZE + 2) * (CHUNK_SIZE + 2) * (CHUNK_SIZE + 2)> blocks,
                            std::vector<MeshFace>& faces)
{
    // rx = faces that point to the right in the x direction
    BlockBitMask* rxBlockFaceBitMask = new BlockBitMask[CHUNK_SIZE * CHUNK_SIZE];
    BlockBitMask* lxBlockFaceBitMask = new BlockBitMask[CHUNK_SIZE * CHUNK_SIZE];
    BlockBitMask* ryBlockFaceBitMask = new BlockBitMask[CHUNK_SIZE * CHUNK_SIZE];
    BlockBitMask* lyBlockFaceBitMask = new BlockBitMask[CHUNK_SIZE * CHUNK_SIZE];
    BlockBitMask* rzBlockFaceBitMask = new BlockBitMask[CHUNK_SIZE * CHUNK_SIZE];
    BlockBitMask* lzBlockFaceBitMask = new BlockBitMask[CHUNK_SIZE * CHUNK_SIZE];

    for (int z = 1; z < CHUNK_SIZE + 1; z++) {
        for (int y = 1; y < CHUNK_SIZE + 1; y++) {
            for (int x = 1; x < CHUNK_SIZE + 1; x++) {
                BlockType block = blocks[blockArrayLocToIndex(x, y, z)];

                if (getRenderType(block) == BlockRenderType::transparent) {
                    assertm(false, "custom block not implemented");
                    //renderNonSolidBlock(x, y, z, block, vertices);
                }
                if (getRenderType(block) == BlockRenderType::custom) {
                    meshCustomBlock(x - 1, y - 1, z - 1, block, faces);
                }
            }
        }
    }

    for (int y = 1; y < CHUNK_SIZE + 1; y++) {
        for (int z = 1; z < CHUNK_SIZE + 1; z++) {
            BlockBitMask blockBitMask = 0;

            for (int x = 0; x < CHUNK_SIZE + 2; x++) {
                BlockType block = blocks[blockArrayLocToIndex(x, y, z)];
                blockBitMask |= (BlockBitMask)isBlockSolid(block) << x;
            }

            rxBlockFaceBitMask[chunkLocationToIndex(y - 1, z - 1)] = getBlockFacesLeftShift(blockBitMask) >> 1;
            lxBlockFaceBitMask[chunkLocationToIndex(y - 1, z - 1)] = getBlockFacesRightShift(blockBitMask) >> 1;
        }
    }

    for (int x = 1; x < CHUNK_SIZE + 1; x++) {
        for (int z = 1; z < CHUNK_SIZE + 1; z++) {
            BlockBitMask blockBitMask = 0;

            for (int y = 0; y < CHUNK_SIZE + 2; y++) {
                BlockType block = blocks[blockArrayLocToIndex(x, y, z)];
                blockBitMask |= (BlockBitMask)isBlockSolid(block) << y;
            }

            ryBlockFaceBitMask[chunkLocationToIndex(x - 1, z - 1)] = getBlockFacesLeftShift(blockBitMask) >> 1;
            lyBlockFaceBitMask[chunkLocationToIndex(x - 1, z - 1)] = getBlockFacesRightShift(blockBitMask) >> 1;
        }
    }

    for (int x = 1; x < CHUNK_SIZE + 1; x++) {
        for (int y = 1; y < CHUNK_SIZE + 1; y++) {
            BlockBitMask blockBitMask = 0;

            for (int z = 0; z < CHUNK_SIZE + 2; z++) {
                BlockType block = blocks[blockArrayLocToIndex(x, y, z)];
                blockBitMask |= (BlockBitMask)isBlockSolid(block) << z;
            }

            rzBlockFaceBitMask[chunkLocationToIndex(x - 1, y - 1)] = getBlockFacesLeftShift(blockBitMask) >> 1;
            lzBlockFaceBitMask[chunkLocationToIndex(x - 1, y - 1)] = getBlockFacesRightShift(blockBitMask) >> 1;
        }
    }

    mergeFaces<coordinateSwapZ, addMeshFacePosZ>(rzBlockFaceBitMask, faces, blocks);
    mergeFaces<coordinateSwapZ, addMeshFaceNegZ>(lzBlockFaceBitMask, faces, blocks);

    mergeFaces<coordinateSwapX, addMeshFacePosX>(rxBlockFaceBitMask, faces, blocks);
    mergeFaces<coordinateSwapX, addMeshFaceNegX>(lxBlockFaceBitMask, faces, blocks);

    mergeFaces<coordinateSwapY, addMeshFacePosY>(ryBlockFaceBitMask, faces, blocks);
    mergeFaces<coordinateSwapY, addMeshFaceNegY>(lyBlockFaceBitMask, faces, blocks);

    delete[] rxBlockFaceBitMask;
    delete[] lxBlockFaceBitMask;
    delete[] ryBlockFaceBitMask;
    delete[] lyBlockFaceBitMask;
    delete[] rzBlockFaceBitMask;
    delete[] lzBlockFaceBitMask;
}

uint32_t packR8G8B8A8_UNORM(float r, float g, float b, float a)
{
    int ri = round(std::clamp(r, 0.0f, 1.0f) * 255);
    int gi = round(std::clamp(g, 0.0f, 1.0f) * 255);
    int bi = round(std::clamp(b, 0.0f, 1.0f) * 255);
    int ai = round(std::clamp(a, 0.0f, 1.0f) * 255);
    return ri | (gi << 8) | (bi << 16) | (ai << 24);
}

uint16_t packR8G8_UNORM(float r, float g)
{
    int ri = round(std::clamp(r, 0.0f, 1.0f) * 255);
    int gi = round(std::clamp(g, 0.0f, 1.0f) * 255);
    return ri | (gi << 8);
}

uint32_t packA2R10G10B10_UNORM(float r, float g, float b, float a)
{
    int ri = round(std::clamp(r, 0.0f, 1.0f) * 1023);
    int gi = round(std::clamp(g, 0.0f, 1.0f) * 1023);
    int bi = round(std::clamp(b, 0.0f, 1.0f) * 1023);
    int ai = round(std::clamp(a, 0.0f, 1.0f) * 3);
    return ri | (gi << 10) | (bi << 20) | (ai << 30);
}

Vertex packVertex(float x,
                  float y,
                  float z,
                  float normalX,
                  float normalY,
                  float normalZ,
                  int shadow,
                  float u,
                  float v,
                  uint32_t textureIndex)
{
    return Vertex(
        packR8G8B8A8_UNORM(x / (float)CHUNK_SIZE, y / (float)CHUNK_SIZE, z / (float)CHUNK_SIZE, normalX + 1.0f / 2.0f),
        packR8G8_UNORM(normalY + 1.0f / 2.0f, normalZ + 1.0f / 2.0f),
        packA2R10G10B10_UNORM(u / 32.0f, v / 32.0f, textureIndex / 1023.0f, shadow));
}

void binaryGreedyMeshChunk4(WorldManager& worldManager, glm::i32vec3 chunkLocation, std::vector<Vertex>& vertices)
{
    Chunk* chunk = &worldManager.chunks[chunkLocation];

    if (!chunk->containsDifferentBlocks && !isBlockSolid(chunk->blocks[0])) {
        return;
    }

    auto debugStartWait = std::chrono::high_resolution_clock::now();

    Chunk* chunkPX = nullptr;
    Chunk* chunkNX = nullptr;
    Chunk* chunkPY = nullptr;
    Chunk* chunkNY = nullptr;
    Chunk* chunkPZ = nullptr;
    Chunk* chunkNZ = nullptr;

    glm::i32vec3 pX = glm::i32vec3(chunkLocation.x + 1, chunkLocation.y, chunkLocation.z);
    glm::i32vec3 nX = glm::i32vec3(chunkLocation.x - 1, chunkLocation.y, chunkLocation.z);
    glm::i32vec3 pY = glm::i32vec3(chunkLocation.x, chunkLocation.y + 1, chunkLocation.z);
    glm::i32vec3 nY = glm::i32vec3(chunkLocation.x, chunkLocation.y - 1, chunkLocation.z);
    glm::i32vec3 pZ = glm::i32vec3(chunkLocation.x, chunkLocation.y, chunkLocation.z + 1);
    glm::i32vec3 nZ = glm::i32vec3(chunkLocation.x, chunkLocation.y, chunkLocation.z - 1);

    if (worldManager.chunks.contains(pX)) {
        chunkPX = &worldManager.chunks[pX];
    }
    if (worldManager.chunks.contains(nX)) {
        chunkNX = &worldManager.chunks[nX];
    }
    if (worldManager.chunks.contains(pY)) {
        chunkPY = &worldManager.chunks[pY];
    }
    if (worldManager.chunks.contains(nY)) {
        chunkNY = &worldManager.chunks[nY];
    }
    if (worldManager.chunks.contains(pZ)) {
        chunkPZ = &worldManager.chunks[pZ];
    }
    if (worldManager.chunks.contains(nZ)) {
        chunkNZ = &worldManager.chunks[nZ];
    }

    if (chunkPX == nullptr || chunkNX == nullptr || chunkPY == nullptr || chunkNY == nullptr || chunkPZ == nullptr ||
        chunkNZ == nullptr) {
        throw std::runtime_error("Chunk not loaded when it should be!");
    }

    std::array<BlockType, (CHUNK_SIZE + 2) * (CHUNK_SIZE + 2) * (CHUNK_SIZE + 2)> blocks =
        std::array<BlockType, (CHUNK_SIZE + 2) * (CHUNK_SIZE + 2) * (CHUNK_SIZE + 2)>();
    blocks.fill(BlockType::maxEnum);

    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_SIZE; y++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                blocks[blockArrayLocToIndex(x + 1, y + 1, z + 1)] = chunkGetBlockAtLocation(x, y, z, chunk);
            }
        }
    }
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_SIZE; y++) {
            blocks[blockArrayLocToIndex(x + 1, y + 1, 0)] = chunkGetBlockAtLocation(x, y, CHUNK_SIZE - 1, chunkNZ);
            blocks[blockArrayLocToIndex(x + 1, y + 1, CHUNK_SIZE + 1)] = chunkGetBlockAtLocation(x, y, 0, chunkPZ);
        }
    }
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            blocks[blockArrayLocToIndex(x + 1, 0, z + 1)] = chunkGetBlockAtLocation(x, CHUNK_SIZE - 1, z, chunkNY);
            blocks[blockArrayLocToIndex(x + 1, CHUNK_SIZE + 1, z + 1)] = chunkGetBlockAtLocation(x, 0, z, chunkPY);
        }
    }
    for (int y = 0; y < CHUNK_SIZE; y++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            blocks[blockArrayLocToIndex(0, y + 1, z + 1)] = chunkGetBlockAtLocation(CHUNK_SIZE - 1, y, z, chunkNX);
            blocks[blockArrayLocToIndex(CHUNK_SIZE + 1, y + 1, z + 1)] = chunkGetBlockAtLocation(0, y, z, chunkPX);
        }
    }
    for (int x = 0; x < CHUNK_SIZE + 2; x++) {
        for (int y = 0; y < CHUNK_SIZE + 2; y++) {
            for (int z = 0; z < CHUNK_SIZE + 2; z++) {
                bool a = (x == 0) || (x == CHUNK_SIZE + 1);
                bool b = (y == 0) || (y == CHUNK_SIZE + 1);
                bool c = (z == 0) || (z == CHUNK_SIZE + 1);

                bool notOnEdge = !((a && b) || (a && c) || (b && c));
                if ((blocks[blockArrayLocToIndex(x, y, z)] == BlockType::maxEnum) && notOnEdge) {
                    std::cout << "BLOCK NOT SET " << x << " " << y << " " << z << std::endl;
                }
            }
        }
    }
    std::vector<MeshFace> faces = std::vector<MeshFace>();
    binaryGreedyMeshChunk3(blocks, faces);

    for (MeshFace& face : faces) {
        for (int i = 0; i < 4; i++) {
            // clang-format off
            vertices.push_back(packVertex(
                face.location[i].x, face.location[i].y, face.location[i].z,
                0, 0, 0,
                0,
                face.uv[i].x, face.uv[i].y,
                face.textureLayer
            ));
            // clang-format on
        }
    }

    auto debugEndWait = std::chrono::high_resolution_clock::now();
    debugMenuGlobals.chunkGenTimeTotal +=
        std::chrono::duration<float, std::chrono::microseconds::period>(debugEndWait - debugStartWait).count();
    debugMenuGlobals.chunksGenerated += 1;
}

// clang-format off

void addVerticesRight(BlockType blockType, glm::i32vec3 blockLocation, int lenght, int height, std::vector<Vertex>& vertices)
{
    float textureArrayIndex = getBlockTextureLayer(blockType, 0);
 
    vertices.push_back(packVertex(
        blockLocation.x + 1, blockLocation.y, blockLocation.z,
        1, 0, 0,
        1,
        0.0f + UV_EDGE_CORRECTION, 1.0f * height - UV_EDGE_CORRECTION,
        textureArrayIndex
    ));
    vertices.push_back(packVertex(
        blockLocation.x + 1, blockLocation.y, blockLocation.z + lenght,
        1, 0, 0,
        1,
        1.0f * lenght - UV_EDGE_CORRECTION, 1.0f * height - UV_EDGE_CORRECTION,
        textureArrayIndex
    ));
    vertices.push_back(packVertex(
        blockLocation.x + 1, blockLocation.y + height, blockLocation.z + lenght,
        1, 0, 0,
        1,
        1.0f * lenght - UV_EDGE_CORRECTION, 0.0f + UV_EDGE_CORRECTION,
        textureArrayIndex
    ));
    vertices.push_back(packVertex(
        blockLocation.x + 1, blockLocation.y + height, blockLocation.z,
        1, 0, 0,
        1,
        0.0f + UV_EDGE_CORRECTION, 0.0f + UV_EDGE_CORRECTION,
        textureArrayIndex
    ));
}

void addVerticesLeft(BlockType blockType, glm::i32vec3 blockLocation, int lenght, int height, std::vector<Vertex>& vertices)
{
    float textureArrayIndex = getBlockTextureLayer(blockType, 1);
    vertices.push_back(packVertex(
        blockLocation.x, blockLocation.y, blockLocation.z,
        -1, 0, 0,
        0,
        0.0f + UV_EDGE_CORRECTION, 1.0f * height - UV_EDGE_CORRECTION,
        textureArrayIndex
    ));
    vertices.push_back(packVertex(
        blockLocation.x, blockLocation.y + height, blockLocation.z,
        -1, 0, 0,
        0,
        0.0f + UV_EDGE_CORRECTION, 0.0f + UV_EDGE_CORRECTION,
        textureArrayIndex
    ));
    vertices.push_back(packVertex(
        blockLocation.x, blockLocation.y + height, blockLocation.z + lenght,
        -1, 0, 0,
        0,
        1.0f * lenght - UV_EDGE_CORRECTION, 0.0f + UV_EDGE_CORRECTION,
        textureArrayIndex
    ));
    vertices.push_back(packVertex(
        blockLocation.x, blockLocation.y, blockLocation.z + lenght,
        -1, 0, 0,
        0,
        1.0f * lenght - UV_EDGE_CORRECTION, 1.0f * height - UV_EDGE_CORRECTION,
        textureArrayIndex
    ));
}

void addVerticesUp(BlockType blockType, glm::i32vec3 blockLocation, int width, int lenght, std::vector<Vertex>& vertices)
{
    float textureArrayIndex = getBlockTextureLayer(blockType, 2); 
    vertices.push_back(packVertex(
        blockLocation.x, blockLocation.y + 1, blockLocation.z,
        0, 1, 0,
        0,
        0.0f + UV_EDGE_CORRECTION, 1.0f * lenght - UV_EDGE_CORRECTION,
        textureArrayIndex
    ));
    vertices.push_back(packVertex(
        blockLocation.x + width, blockLocation.y + 1, blockLocation.z,
        0, 1, 0,
        0,
        1.0f * width - UV_EDGE_CORRECTION, 1.0f * lenght - UV_EDGE_CORRECTION,
        textureArrayIndex
    ));
    vertices.push_back(packVertex(
        blockLocation.x + width, blockLocation.y + 1, blockLocation.z + lenght,
        0, 1, 0,
        0,
        1.0f * width - UV_EDGE_CORRECTION, 0.0f + UV_EDGE_CORRECTION,
        textureArrayIndex
    ));
    vertices.push_back(packVertex(
        blockLocation.x, blockLocation.y + 1, blockLocation.z + lenght,
        0, 1, 0,
        0,
        0.0f + UV_EDGE_CORRECTION, 0.0f + UV_EDGE_CORRECTION,
        textureArrayIndex
    ));
}

void addVerticesDown(BlockType blockType, glm::i32vec3 blockLocation, int width, int lenght, std::vector<Vertex>& vertices)
{
    float textureArrayIndex = getBlockTextureLayer(blockType, 3);
    vertices.push_back(packVertex(
        blockLocation.x, blockLocation.y, blockLocation.z,
        0, -1, 0,
        1,
        0.0f + UV_EDGE_CORRECTION, 1.0f * lenght - UV_EDGE_CORRECTION,
        textureArrayIndex
    ));
    vertices.push_back(packVertex(
        blockLocation.x, blockLocation.y, blockLocation.z + lenght,
        0, -1, 0,
        1,
        0.0f + UV_EDGE_CORRECTION, 0.0f + UV_EDGE_CORRECTION,
        textureArrayIndex
    ));
    vertices.push_back(packVertex(
        blockLocation.x + width, blockLocation.y, blockLocation.z + lenght,
        0, -1, 0,
        1,
        1.0f * width - UV_EDGE_CORRECTION, 0.0f + UV_EDGE_CORRECTION,
        textureArrayIndex
    ));
    vertices.push_back(packVertex(
        blockLocation.x + width, blockLocation.y, blockLocation.z,
        0, -1, 0,
        1,
        1.0f * width - UV_EDGE_CORRECTION, 1.0f * lenght - UV_EDGE_CORRECTION,
        textureArrayIndex
    ));
}

void addVerticesForward(BlockType blockType, glm::i32vec3 blockLocation, int width, int height, std::vector<Vertex>& vertices)
{
    float textureArrayIndex = getBlockTextureLayer(blockType, 4);
    vertices.push_back(packVertex(
        blockLocation.x, blockLocation.y, blockLocation.z + 1,
        0, 0, 1,
        1,
        0.0f + UV_EDGE_CORRECTION, 1.0f * height - UV_EDGE_CORRECTION,
        textureArrayIndex
    ));
    vertices.push_back(packVertex(
        blockLocation.x, blockLocation.y + height, blockLocation.z + 1,
        0, 0, 1,
        1,
        0.0f + UV_EDGE_CORRECTION, 0.0f + UV_EDGE_CORRECTION,
        textureArrayIndex
    ));
    vertices.push_back(packVertex(
        blockLocation.x + width, blockLocation.y + height, blockLocation.z + 1,
        0, 0, 1,
        1,
        1.0f * width - UV_EDGE_CORRECTION, 0.0f + UV_EDGE_CORRECTION,
        textureArrayIndex
    ));
    vertices.push_back(packVertex(
        blockLocation.x + width, blockLocation.y, blockLocation.z + 1,
        0, 0, 1,
        1,
        1.0f * width - UV_EDGE_CORRECTION, 1.0f * height - UV_EDGE_CORRECTION,
        textureArrayIndex
    ));
}

void addVerticesBackward(BlockType blockType, glm::i32vec3 blockLocation, int width, int height, std::vector<Vertex>& vertices)
{
    float textureArrayIndex = getBlockTextureLayer(blockType, 5); 
    vertices.push_back(packVertex(
        blockLocation.x, blockLocation.y, blockLocation.z,
        0, 0, -1,
        0,
        0.0f + UV_EDGE_CORRECTION, 1.0f * height - UV_EDGE_CORRECTION,
        textureArrayIndex
    ));
    vertices.push_back(packVertex(
        blockLocation.x + width, blockLocation.y, blockLocation.z,
        0, 0, -1,
        0,
        1.0f * width - UV_EDGE_CORRECTION, 1.0f * height - UV_EDGE_CORRECTION,
        textureArrayIndex
    ));
    vertices.push_back(packVertex(
        blockLocation.x + width, blockLocation.y + height, blockLocation.z,
        0, 0, -1,
        0,
        1.0f * width - UV_EDGE_CORRECTION, 0.0f + UV_EDGE_CORRECTION,
        textureArrayIndex
    ));
    vertices.push_back(packVertex(
        blockLocation.x, blockLocation.y + height, blockLocation.z,
        0, 0, -1,
        0,
        0.0f + UV_EDGE_CORRECTION, 0.0f + UV_EDGE_CORRECTION,
        textureArrayIndex
    ));
}

// clang-format on

void renderNonSolidBlock(int x, int y, int z, BlockType blockType, std::vector<Vertex>& vertices)
{
    glm::ivec3 blockLocation = glm::ivec3(x, y, z);

    addVerticesRight(blockType, blockLocation, 1, 1, vertices);
    addVerticesLeft(blockType, blockLocation, 1, 1, vertices);
    addVerticesUp(blockType, blockLocation, 1, 1, vertices);
    addVerticesDown(blockType, blockLocation, 1, 1, vertices);
    addVerticesForward(blockType, blockLocation, 1, 1, vertices);
    addVerticesBackward(blockType, blockLocation, 1, 1, vertices);
}

void renderCustomBlock(int x, int y, int z, BlockType blockType, std::vector<Vertex>& vertices)
{
    float textureArrayIndex = getBlockTextureLayer(blockType, 0);

    glm::vec3 blockLocation = glm::ivec3(x, y, z);
    std::vector<glm::vec3> vertexOffsets = blockCustomRenderVertexOffsets.at(blockType);

    static glm::vec2 uvCoordinates[4] = {
        glm::vec2(0.0f, 1.0f), glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 0.0f), glm::vec2(0.0f, 0.0f)};

    for (size_t i = 0; i < vertexOffsets.size(); i++) {
        glm::vec3 blockPos = blockLocation + vertexOffsets[i];
        glm::vec2 blockUV = uvCoordinates[i % 4];
        vertices.push_back(
            packVertex(blockPos.x, blockPos.y, blockPos.z, 0, 0, 0, 0, blockUV.x, blockUV.y, textureArrayIndex));
    }
}

void xDirectionMergeFaces(int y,
                          int z,
                          Chunk* chunk,
                          BlockBitMask* xBlockFaceBitMask,
                          std::vector<Vertex>& vertices,
                          bool directionIsPositive)
{
    BlockBitMask blockBitMask = xBlockFaceBitMask[chunkLocationToIndex(y, z)];
    while (blockBitMask != 0) {
        int height = 1;
        int lenght = 1;
        BlockBitMask startingBlock = blockBitMask & (0 - blockBitMask); // get least significant bit

        // The location takes into account that the neighboring chunk block is at index 0.
        // The first block in this chunk has the value x = 1.
        int x = LovestSignificantBitIndex(startingBlock);
        BlockType originalBlockType = chunk->blocks[chunkLocationToIndex(x - 1, y, z)];

        for (int yExpansion = 1; yExpansion < CHUNK_SIZE - y; yExpansion++) {

            if (((blockBitMask & xBlockFaceBitMask[chunkLocationToIndex(y + yExpansion, z)]) == 0) ||
                (originalBlockType != chunk->blocks[chunkLocationToIndex(x - 1, y + yExpansion, z)])) {
                break;
            }
            // Turn off blocks that can be merged.
            xBlockFaceBitMask[chunkLocationToIndex(y + yExpansion, z)] ^= startingBlock;
            height++;
        }

        for (int zExpansion = 1; zExpansion < CHUNK_SIZE - z; zExpansion++) {
            bool cantExpand = false;
            for (int yExpansion = 0; yExpansion < height; yExpansion++) {
                if ((blockBitMask & xBlockFaceBitMask[chunkLocationToIndex(y + yExpansion, z + zExpansion)]) == 0 ||
                    originalBlockType != chunk->blocks[chunkLocationToIndex(x - 1, y + yExpansion, z + zExpansion)]) {
                    cantExpand = true;
                    break;
                }
            }
            if (cantExpand) {
                break;
            }
            // Turn off blocks that can be merged.
            for (int yExpansion = 0; yExpansion < height; yExpansion++) {
                xBlockFaceBitMask[chunkLocationToIndex(y + yExpansion, z + zExpansion)] ^= startingBlock;
            }
            lenght++;
        }

        blockBitMask ^= startingBlock; // turn off the bit that was used

        if (directionIsPositive) {
            addVerticesRight(originalBlockType, glm::i32vec3(x - 1, y, z), lenght, height, vertices);
        }
        else {
            addVerticesLeft(originalBlockType, glm::i32vec3(x - 1, y, z), lenght, height, vertices);
        }
    }
}

void yDirectionMergeFaces(int x,
                          int z,
                          Chunk* chunk,
                          BlockBitMask* yBlockFaceBitMask,
                          std::vector<Vertex>& vertices,
                          bool directionIsPositive)
{
    BlockBitMask blockBitMask = yBlockFaceBitMask[chunkLocationToIndex(x, z)];
    while (blockBitMask != 0) {
        int width = 1;
        int lenght = 1;
        BlockBitMask startingBlock = blockBitMask & (0 - blockBitMask); // get least significant bit

        // The location takes into account that the neighboring chunk block is at index 0.
        // The first block in this chunk has the value y = 1.
        int y = LovestSignificantBitIndex(startingBlock);
        BlockType originalBlockType = chunk->blocks[chunkLocationToIndex(x, y - 1, z)];

        for (int xExpansion = 1; xExpansion < CHUNK_SIZE - x; xExpansion++) {

            if (((blockBitMask & yBlockFaceBitMask[chunkLocationToIndex(x + xExpansion, z)]) == 0) ||
                (originalBlockType != chunk->blocks[chunkLocationToIndex(x + xExpansion, y - 1, z)])) {
                break;
            }
            // Turn off blocks that can be merged.
            yBlockFaceBitMask[chunkLocationToIndex(x + xExpansion, z)] ^= startingBlock;
            width++;
        }

        for (int zExpansion = 1; zExpansion < CHUNK_SIZE - z; zExpansion++) {
            bool cantExpand = false;
            for (int xExpansion = 0; xExpansion < width; xExpansion++) {
                if ((blockBitMask & yBlockFaceBitMask[chunkLocationToIndex(x + xExpansion, z + zExpansion)]) == 0 ||
                    originalBlockType != chunk->blocks[chunkLocationToIndex(x + xExpansion, y - 1, z + zExpansion)]) {
                    cantExpand = true;
                    break;
                }
            }
            if (cantExpand) {
                break;
            }
            // Turn off blocks that can be merged.
            for (int xExpansion = 0; xExpansion < width; xExpansion++) {
                yBlockFaceBitMask[chunkLocationToIndex(x + xExpansion, z + zExpansion)] ^= startingBlock;
            }
            lenght++;
        }

        blockBitMask ^= startingBlock; // turn off the bit that was used

        if (directionIsPositive) {
            addVerticesUp(originalBlockType, glm::i32vec3(x, y - 1, z), width, lenght, vertices);
        }
        else {
            addVerticesDown(originalBlockType, glm::i32vec3(x, y - 1, z), width, lenght, vertices);
        }
    }
}

void zDirectionMergeFaces(int x,
                          int y,
                          Chunk* chunk,
                          BlockBitMask* zBlockFaceBitMask,
                          std::vector<Vertex>& vertices,
                          bool directionIsPositive)
{
    BlockBitMask blockBitMask = zBlockFaceBitMask[chunkLocationToIndex(x, y)];
    while (blockBitMask != 0) {
        int width = 1;
        int height = 1;
        BlockBitMask startingBlock = blockBitMask & (0 - blockBitMask); // get least significant bit

        // The location takes into account that the neighboring chunk block is at index 0.
        // The first block in this chunk has the value z = 1.
        int z = LovestSignificantBitIndex(startingBlock);
        BlockType originalBlockType = chunk->blocks[chunkLocationToIndex(x, y, z - 1)];

        for (int xExpansion = 1; xExpansion < CHUNK_SIZE - x; xExpansion++) {

            if (((blockBitMask & zBlockFaceBitMask[chunkLocationToIndex(x + xExpansion, y)]) == 0) ||
                (originalBlockType != chunk->blocks[chunkLocationToIndex(x + xExpansion, y, z - 1)])) {
                break;
            }
            // Turn off blocks that can be merged.
            zBlockFaceBitMask[chunkLocationToIndex(x + xExpansion, y)] &= ~startingBlock;
            width++;
        }

        for (int yExpansion = 1; yExpansion < CHUNK_SIZE - y; yExpansion++) {
            bool cantExpand = false;
            for (int xExpansion = 0; xExpansion < width; xExpansion++) {
                if ((blockBitMask & zBlockFaceBitMask[chunkLocationToIndex(x + xExpansion, y + yExpansion)]) == 0 ||
                    originalBlockType != chunk->blocks[chunkLocationToIndex(x + xExpansion, y + yExpansion, z - 1)]) {
                    cantExpand = true;
                    break;
                }
            }
            if (cantExpand) {
                break;
            }
            // Turn off blocks that can be merged.
            for (int xExpansion = 0; xExpansion < width; xExpansion++) {
                zBlockFaceBitMask[chunkLocationToIndex(x + xExpansion, y + yExpansion)] &= ~startingBlock;
            }
            height++;
        }

        blockBitMask &= ~startingBlock; // turn off the bit that was used

        if (directionIsPositive) {
            addVerticesForward(originalBlockType, glm::i32vec3(x, y, z - 1), width, height, vertices);
        }
        else {
            addVerticesBackward(originalBlockType, glm::i32vec3(x, y, z - 1), width, height, vertices);
        }
    }
}

void binaryGreedyMeshChunk2(std::array<BlockType, (CHUNK_SIZE + 2) * (CHUNK_SIZE + 2) * (CHUNK_SIZE + 2)> blocks,
                            Chunk* chunk,
                            std::vector<Vertex>& vertices)
{
    auto debugStartWait = std::chrono::high_resolution_clock::now();

    // rx = faces that point to the right in the x direction
    BlockBitMask* rxBlockFaceBitMask = new BlockBitMask[CHUNK_SIZE * CHUNK_SIZE];
    BlockBitMask* lxBlockFaceBitMask = new BlockBitMask[CHUNK_SIZE * CHUNK_SIZE];
    BlockBitMask* ryBlockFaceBitMask = new BlockBitMask[CHUNK_SIZE * CHUNK_SIZE];
    BlockBitMask* lyBlockFaceBitMask = new BlockBitMask[CHUNK_SIZE * CHUNK_SIZE];
    BlockBitMask* rzBlockFaceBitMask = new BlockBitMask[CHUNK_SIZE * CHUNK_SIZE];
    BlockBitMask* lzBlockFaceBitMask = new BlockBitMask[CHUNK_SIZE * CHUNK_SIZE];

    for (int z = 1; z < CHUNK_SIZE + 1; z++) {
        for (int y = 1; y < CHUNK_SIZE + 1; y++) {
            for (int x = 1; x < CHUNK_SIZE + 1; x++) {
                BlockType block = blocks[blockArrayLocToIndex(x, y, z)];

                if (getRenderType(block) == BlockRenderType::transparent) {
                    renderNonSolidBlock(x, y, z, block, vertices);
                }
                if (getRenderType(block) == BlockRenderType::custom) {
                    renderCustomBlock(x, y, z, block, vertices);
                }
            }
        }
    }

    for (int y = 1; y < CHUNK_SIZE + 1; y++) {
        for (int z = 1; z < CHUNK_SIZE + 1; z++) {
            BlockBitMask blockBitMask = 0;

            for (int x = 0; x < CHUNK_SIZE + 2; x++) {
                BlockType block = blocks[blockArrayLocToIndex(x, y, z)];
                blockBitMask |= (BlockBitMask)isBlockSolid(block) << x;
            }

            rxBlockFaceBitMask[chunkLocationToIndex(y - 1, z - 1)] = getBlockFacesLeftShift(blockBitMask);
            lxBlockFaceBitMask[chunkLocationToIndex(y - 1, z - 1)] = getBlockFacesRightShift(blockBitMask);
        }
    }

    for (int x = 1; x < CHUNK_SIZE + 1; x++) {
        for (int z = 1; z < CHUNK_SIZE + 1; z++) {
            BlockBitMask blockBitMask = 0;

            for (int y = 0; y < CHUNK_SIZE + 2; y++) {
                BlockType block = blocks[blockArrayLocToIndex(x, y, z)];
                blockBitMask |= (BlockBitMask)isBlockSolid(block) << y;
            }

            ryBlockFaceBitMask[chunkLocationToIndex(x - 1, z - 1)] = getBlockFacesLeftShift(blockBitMask);
            lyBlockFaceBitMask[chunkLocationToIndex(x - 1, z - 1)] = getBlockFacesRightShift(blockBitMask);
        }
    }

    for (int x = 1; x < CHUNK_SIZE + 1; x++) {
        for (int y = 1; y < CHUNK_SIZE + 1; y++) {
            BlockBitMask blockBitMask = 0;

            for (int z = 0; z < CHUNK_SIZE + 2; z++) {
                BlockType block = blocks[blockArrayLocToIndex(x, y, z)];
                blockBitMask |= (BlockBitMask)isBlockSolid(block) << z;
            }

            rzBlockFaceBitMask[chunkLocationToIndex(x - 1, y - 1)] = getBlockFacesLeftShift(blockBitMask);
            lzBlockFaceBitMask[chunkLocationToIndex(x - 1, y - 1)] = getBlockFacesRightShift(blockBitMask);
        }
    }

    for (int i = 0; i < CHUNK_SIZE; i++) {
        for (int k = 0; k < CHUNK_SIZE; k++) {

            xDirectionMergeFaces(i, k, chunk, rxBlockFaceBitMask, vertices, true);
            xDirectionMergeFaces(i, k, chunk, lxBlockFaceBitMask, vertices, false);

            yDirectionMergeFaces(i, k, chunk, ryBlockFaceBitMask, vertices, true);
            yDirectionMergeFaces(i, k, chunk, lyBlockFaceBitMask, vertices, false);

            zDirectionMergeFaces(i, k, chunk, rzBlockFaceBitMask, vertices, true);
            zDirectionMergeFaces(i, k, chunk, lzBlockFaceBitMask, vertices, false);
        }
    }

    delete[] rxBlockFaceBitMask;
    delete[] lxBlockFaceBitMask;
    delete[] ryBlockFaceBitMask;
    delete[] lyBlockFaceBitMask;
    delete[] rzBlockFaceBitMask;
    delete[] lzBlockFaceBitMask;

    auto debugEndWait = std::chrono::high_resolution_clock::now();
    debugMenuGlobals.chunkGenTimeTotal +=
        std::chrono::duration<float, std::chrono::microseconds::period>(debugEndWait - debugStartWait).count();
    debugMenuGlobals.chunksGenerated += 1;
}

void binaryGreedyMeshChunk(WorldManager& worldManager, glm::i32vec3 chunkLocation, std::vector<Vertex>& vertices)
{
    Chunk* chunk = &worldManager.chunks[chunkLocation];

    if (!chunk->containsDifferentBlocks && !isBlockSolid(chunk->blocks[0])) {
        return;
    }
    auto debugStartWait = std::chrono::high_resolution_clock::now();

    Chunk* chunkPX = nullptr;
    Chunk* chunkNX = nullptr;
    Chunk* chunkPY = nullptr;
    Chunk* chunkNY = nullptr;
    Chunk* chunkPZ = nullptr;
    Chunk* chunkNZ = nullptr;

    glm::i32vec3 pX = glm::i32vec3(chunkLocation.x + 1, chunkLocation.y, chunkLocation.z);
    glm::i32vec3 nX = glm::i32vec3(chunkLocation.x - 1, chunkLocation.y, chunkLocation.z);
    glm::i32vec3 pY = glm::i32vec3(chunkLocation.x, chunkLocation.y + 1, chunkLocation.z);
    glm::i32vec3 nY = glm::i32vec3(chunkLocation.x, chunkLocation.y - 1, chunkLocation.z);
    glm::i32vec3 pZ = glm::i32vec3(chunkLocation.x, chunkLocation.y, chunkLocation.z + 1);
    glm::i32vec3 nZ = glm::i32vec3(chunkLocation.x, chunkLocation.y, chunkLocation.z - 1);

    if (worldManager.chunks.contains(pX)) {
        chunkPX = &worldManager.chunks[pX];
    }
    if (worldManager.chunks.contains(nX)) {
        chunkNX = &worldManager.chunks[nX];
    }
    if (worldManager.chunks.contains(pY)) {
        chunkPY = &worldManager.chunks[pY];
    }
    if (worldManager.chunks.contains(nY)) {
        chunkNY = &worldManager.chunks[nY];
    }
    if (worldManager.chunks.contains(pZ)) {
        chunkPZ = &worldManager.chunks[pZ];
    }
    if (worldManager.chunks.contains(nZ)) {
        chunkNZ = &worldManager.chunks[nZ];
    }

    if (chunkPX == nullptr || chunkNX == nullptr || chunkPY == nullptr || chunkNY == nullptr || chunkPZ == nullptr ||
        chunkNZ == nullptr) {
        throw std::runtime_error("Chunk not loaded when it should be!");
    }

    std::array<BlockType, (CHUNK_SIZE + 2) * (CHUNK_SIZE + 2) * (CHUNK_SIZE + 2)> blocks =
        std::array<BlockType, (CHUNK_SIZE + 2) * (CHUNK_SIZE + 2) * (CHUNK_SIZE + 2)>();
    blocks.fill(BlockType::maxEnum);

    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_SIZE; y++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                blocks[blockArrayLocToIndex(x + 1, y + 1, z + 1)] = chunkGetBlockAtLocation(x, y, z, chunk);
            }
        }
    }
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_SIZE; y++) {
            blocks[blockArrayLocToIndex(x + 1, y + 1, 0)] = chunkGetBlockAtLocation(x, y, CHUNK_SIZE - 1, chunkNZ);
            blocks[blockArrayLocToIndex(x + 1, y + 1, CHUNK_SIZE + 1)] = chunkGetBlockAtLocation(x, y, 0, chunkPZ);
        }
    }
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            blocks[blockArrayLocToIndex(x + 1, 0, z + 1)] = chunkGetBlockAtLocation(x, CHUNK_SIZE - 1, z, chunkNY);
            blocks[blockArrayLocToIndex(x + 1, CHUNK_SIZE + 1, z + 1)] = chunkGetBlockAtLocation(x, 0, z, chunkPY);
        }
    }
    for (int y = 0; y < CHUNK_SIZE; y++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            blocks[blockArrayLocToIndex(0, y + 1, z + 1)] = chunkGetBlockAtLocation(CHUNK_SIZE - 1, y, z, chunkNX);
            blocks[blockArrayLocToIndex(CHUNK_SIZE + 1, y + 1, z + 1)] = chunkGetBlockAtLocation(0, y, z, chunkPX);
        }
    }
    for (int x = 0; x < CHUNK_SIZE + 2; x++) {
        for (int y = 0; y < CHUNK_SIZE + 2; y++) {
            for (int z = 0; z < CHUNK_SIZE + 2; z++) {
                bool a = (x == 0) || (x == CHUNK_SIZE + 1);
                bool b = (y == 0) || (y == CHUNK_SIZE + 1);
                bool c = (z == 0) || (z == CHUNK_SIZE + 1);

                bool notOnEdge = !((a && b) || (a && c) || (b && c));
                if ((blocks[blockArrayLocToIndex(x, y, z)] == BlockType::maxEnum) && notOnEdge) {
                    std::cout << "BLOCK NOT SET " << x << " " << y << " " << z << std::endl;
                }
            }
        }
    }
    binaryGreedyMeshChunk2(blocks, chunk, vertices);
}

/*void binaryGreedyMeshChunk(WorldManager& worldManager, glm::i32vec3 chunkLocation, std::vector<Vertex>& vertices)
{
    Chunk* chunk = &worldManager.chunks[chunkLocation];

    if (!chunk->containsDifferentBlocks && !isBlockSolid(chunk->blocks[0])) {
        return;
    }
    auto debugStartWait = std::chrono::high_resolution_clock::now();

    Chunk* chunkPX = nullptr;
    Chunk* chunkNX = nullptr;
    Chunk* chunkPY = nullptr;
    Chunk* chunkNY = nullptr;
    Chunk* chunkPZ = nullptr;
    Chunk* chunkNZ = nullptr;

    glm::i32vec3 pX = glm::i32vec3(chunkLocation.x + 1, chunkLocation.y, chunkLocation.z);
    glm::i32vec3 nX = glm::i32vec3(chunkLocation.x - 1, chunkLocation.y, chunkLocation.z);
    glm::i32vec3 pY = glm::i32vec3(chunkLocation.x, chunkLocation.y + 1, chunkLocation.z);
    glm::i32vec3 nY = glm::i32vec3(chunkLocation.x, chunkLocation.y - 1, chunkLocation.z);
    glm::i32vec3 pZ = glm::i32vec3(chunkLocation.x, chunkLocation.y, chunkLocation.z + 1);
    glm::i32vec3 nZ = glm::i32vec3(chunkLocation.x, chunkLocation.y, chunkLocation.z - 1);

    if (worldManager.chunks.contains(pX)) {
        chunkPX = &worldManager.chunks[pX];
    }
    if (worldManager.chunks.contains(nX)) {
        chunkNX = &worldManager.chunks[nX];
    }
    if (worldManager.chunks.contains(pY)) {
        chunkPY = &worldManager.chunks[pY];
    }
    if (worldManager.chunks.contains(nY)) {
        chunkNY = &worldManager.chunks[nY];
    }
    if (worldManager.chunks.contains(pZ)) {
        chunkPZ = &worldManager.chunks[pZ];
    }
    if (worldManager.chunks.contains(nZ)) {
        chunkNZ = &worldManager.chunks[nZ];
    }

    if (chunkPX == nullptr || chunkNX == nullptr || chunkPY == nullptr || chunkNY == nullptr || chunkPZ == nullptr ||
        chunkNZ == nullptr) {
        throw std::runtime_error("Chunk not loaded when it should be!");
    }

    glm::i32vec3 chunkBlockLocationOffset = glm::ivec3(0, 0, 0); // chunkLocation * CHUNK_SIZE;

    // rx = faces that point to the right in the x direction
    BlockBitMask* rxBlockFaceBitMask = new BlockBitMask[CHUNK_SIZE * CHUNK_SIZE];
    BlockBitMask* lxBlockFaceBitMask = new BlockBitMask[CHUNK_SIZE * CHUNK_SIZE];
    BlockBitMask* ryBlockFaceBitMask = new BlockBitMask[CHUNK_SIZE * CHUNK_SIZE];
    BlockBitMask* lyBlockFaceBitMask = new BlockBitMask[CHUNK_SIZE * CHUNK_SIZE];
    BlockBitMask* rzBlockFaceBitMask = new BlockBitMask[CHUNK_SIZE * CHUNK_SIZE];
    BlockBitMask* lzBlockFaceBitMask = new BlockBitMask[CHUNK_SIZE * CHUNK_SIZE];

    for (int y = 0; y < CHUNK_SIZE; y++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            for (int x = 0; x < CHUNK_SIZE; x++) {
                BlockType block = chunkGetBlockAtLocation(x, y, z, chunk);

                if (getRenderType(block) == BlockRenderType::transparent) {
                    RenderNonSolidBlock(x, y, z, chunkBlockLocationOffset, block, vertices);
                }
                if (getRenderType(block) == BlockRenderType::custom) {
                    RenderCustomBlock(x, y, z, chunkBlockLocationOffset, block, vertices);
                }
            }
        }
    }

    for (int y = 0; y < CHUNK_SIZE; y++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {

            BlockBitMask blockBitMask = 0;
            blockBitMask = (BlockBitMask)isBlockSolid(chunkGetBlockAtLocation(CHUNK_SIZE - 1, y, z, chunkNX));

            for (int x = 0; x < CHUNK_SIZE; x++) {
                BlockType block = chunkGetBlockAtLocation(x, y, z, chunk);
                blockBitMask |= (BlockBitMask)isBlockSolid(block) << (x + 1);
            }
            blockBitMask |= (BlockBitMask)isBlockSolid(chunkGetBlockAtLocation(0, y, z, chunkPX)) << CHUNK_SIZE + 1;

            rxBlockFaceBitMask[chunkLocationToIndex(y, z)] = getBlockFacesLeftShift(blockBitMask);
            lxBlockFaceBitMask[chunkLocationToIndex(y, z)] = getBlockFacesRightShift(blockBitMask);
        }
    }

    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {

            BlockBitMask blockBitMask = 0;
            blockBitMask = (BlockBitMask)isBlockSolid(chunkGetBlockAtLocation(x, CHUNK_SIZE - 1, z, chunkNY));

            for (int y = 0; y < CHUNK_SIZE; y++) {
                blockBitMask |= (BlockBitMask)isBlockSolid(chunk->blocks[chunkLocationToIndex(x, y, z)]) << (y + 1);
            }
            blockBitMask |= (BlockBitMask)isBlockSolid(chunkGetBlockAtLocation(x, 0, z, chunkPY)) << CHUNK_SIZE + 1;

            ryBlockFaceBitMask[chunkLocationToIndex(x, z)] = getBlockFacesLeftShift(blockBitMask);
            lyBlockFaceBitMask[chunkLocationToIndex(x, z)] = getBlockFacesRightShift(blockBitMask);
        }
    }

    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_SIZE; y++) {

            BlockBitMask blockBitMask = 0;

            blockBitMask = (BlockBitMask)isBlockSolid(chunkGetBlockAtLocation(x, y, CHUNK_SIZE - 1, chunkNZ));

            for (int z = 0; z < CHUNK_SIZE; z++) {
                blockBitMask |= (BlockBitMask)isBlockSolid(chunk->blocks[chunkLocationToIndex(x, y, z)]) << (z + 1);
            }
            blockBitMask |= (BlockBitMask)isBlockSolid(chunkGetBlockAtLocation(x, y, 0, chunkPZ)) << CHUNK_SIZE + 1;

            rzBlockFaceBitMask[chunkLocationToIndex(x, y)] = getBlockFacesLeftShift(blockBitMask);
            lzBlockFaceBitMask[chunkLocationToIndex(x, y)] = getBlockFacesRightShift(blockBitMask);
        }
    }

    // TODO: test if seperate loops is faster because of cache locality
    for (int i = 0; i < CHUNK_SIZE; i++) {
        for (int k = 0; k < CHUNK_SIZE; k++) {

            xDirectionMergeFaces(i, k, chunk, rxBlockFaceBitMask, vertices, chunkBlockLocationOffset, true);
            xDirectionMergeFaces(i, k, chunk, lxBlockFaceBitMask, vertices, chunkBlockLocationOffset, false);

            yDirectionMergeFaces(i, k, chunk, ryBlockFaceBitMask, vertices, chunkBlockLocationOffset, true);
            yDirectionMergeFaces(i, k, chunk, lyBlockFaceBitMask, vertices, chunkBlockLocationOffset, false);

            zDirectionMergeFaces(i, k, chunk, rzBlockFaceBitMask, vertices, chunkBlockLocationOffset, true);
            zDirectionMergeFaces(i, k, chunk, lzBlockFaceBitMask, vertices, chunkBlockLocationOffset, false);
        }
    }

    delete[] rxBlockFaceBitMask;
    delete[] lxBlockFaceBitMask;
    delete[] ryBlockFaceBitMask;
    delete[] lyBlockFaceBitMask;
    delete[] rzBlockFaceBitMask;
    delete[] lzBlockFaceBitMask;

    auto debugEndWait = std::chrono::high_resolution_clock::now();
    debugMenuGlobals.chunkGenTimeTotal +=
        std::chrono::duration<float, std::chrono::microseconds::period>(debugEndWait - debugStartWait).count();
    debugMenuGlobals.chunksGenerated += 1;
}*/
