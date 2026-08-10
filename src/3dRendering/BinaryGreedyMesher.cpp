#include "BinaryGreedyMesher.hpp"

#include <numbers>
#include <bitset>
#include <algorithm>

#include "BlockTexCoordinateLookup.hpp"
#include "World/BlockDataLookup.hpp"
#include "World/Chunk.hpp"
#include "assertm.hpp"

#include <bit>

typedef uint64_t BlockBitMask;

int LovestSignificantBitIndex(uint64_t bitboard) { return std::countr_zero(bitboard); }

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

static int blockArrayLocToIndex(int x, int y, int z)
{
    return x + y * (CHUNK_SIZE + 2) + z * (CHUNK_SIZE + 2) * (CHUNK_SIZE + 2);
}

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

            while (blockBitMask != 0) {
                int width = 1;
                int height = 1;
                BlockBitMask startingBlock = blockBitMask & (0 - blockBitMask); // get least significant bit
                //hasFace[x + y * CHUNK_SIZE] ^= startingBlock; // turn off used bit mask  TODO: might be unnecessary
                int z = LovestSignificantBitIndex(startingBlock);

                BlockType originalBlockType = blocks[blockArrayLookupTemplate<coordinateSwap>(x + 1, y + 1, z + 1)];

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

void blockArrayMesher(std::array<BlockType, (CHUNK_SIZE + 2) * (CHUNK_SIZE + 2) * (CHUNK_SIZE + 2)> blocks,
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

/*void renderNonSolidBlock(int x, int y, int z, BlockType blockType, std::vector<Vertex>& vertices)
{
    glm::ivec3 blockLocation = glm::ivec3(x, y, z);

    addVerticesRight(blockType, blockLocation, 1, 1, vertices);
    addVerticesLeft(blockType, blockLocation, 1, 1, vertices);
    addVerticesUp(blockType, blockLocation, 1, 1, vertices);
    addVerticesDown(blockType, blockLocation, 1, 1, vertices);
    addVerticesForward(blockType, blockLocation, 1, 1, vertices);
    addVerticesBackward(blockType, blockLocation, 1, 1, vertices);
}*/
