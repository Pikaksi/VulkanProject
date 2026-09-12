#include "chunkMesher.hpp"

#include <chrono>
#include <algorithm>

#include "BlockType.hpp"
#include "Chunk.hpp"
#include "DebugMenu.hpp"
#include "BlockDataLookup.hpp"
#include "BinaryGreedyMesher.hpp"
#include "assertm.hpp"

static int blockArrayLocToIndex(int x, int y, int z)
{
    return x + y * (CHUNK_SIZE + 2) + z * (CHUNK_SIZE + 2) * (CHUNK_SIZE + 2);
}

uint32_t pack4x8(int a, int b, int c, int d) { return a | (b << 8) | (c << 16) | (d << 24); }
uint32_t pack3x10(int a, int b, int c) { return a | (b << 10) | (c << 20); }

int packUint8(float a) { return round(std::clamp(a, 0.0f, 255.0f)); }
int packUint10VertexCoordinate(float a) { return round(std::clamp(a * 16.0f, 0.0f, 1023.0f)); }

int packUnorm8(float a) { return round(std::clamp(a, 0.0f, 1.0f) * 255.0f); }
int packSnorm8(float a) { return round(std::clamp(a, -1.0f, 1.0f) * 127.0f); }

/*uint32_t packR8G8B8A8_UNORM(float r, float g, float b, float a)
{
    int ri = round(std::clamp(r, 0.0f, 1.0f) * 255);
    int gi = round(std::clamp(g, 0.0f, 1.0f) * 255);
    int bi = round(std::clamp(b, 0.0f, 1.0f) * 255);
    int ai = round(std::clamp(a, 0.0f, 1.0f) * 255);
    return ri | (gi << 8) | (bi << 16) | (ai << 24);
}

uint32_t packB8A8_UINT(float r, float g, float b, float a)
{
    int ri = round(std::clamp(r, 0.0f, 255.0f));
    int gi = round(std::clamp(g, 0.0f, 255.0f));
    int bi = round(std::clamp(b, 0.0f, 255.0f));
    int ai = round(std::clamp(a, 0.0f, 255.0f));
    return ri | (gi << 8) | (bi << 16) | (ai << 24);
}

uint32_t packR8G8_UNORM_B8A8_UINT(float r, float g, float b, float a)
{
    int ri = round(std::clamp(r, 0.0f, 1.0f) * 255);
    int gi = round(std::clamp(g, 0.0f, 1.0f) * 255);
    int bi = round(std::clamp(b, 0.0f, 255.0f));
    int ai = round(std::clamp(a, 0.0f, 255.0f));
    return ri | (gi << 8) | (bi << 16) | (ai << 24);
}

uint32_t packR8G8_UNORM_B8A8_UINT(float r, float g, float b, float a)
{
    int ri = round(std::clamp(r, 0.0f, 1.0f) * 255);
    int gi = round(std::clamp(g, 0.0f, 1.0f) * 255);
    int bi = round(std::clamp(b, 0.0f, 255.0f));
    int ai = round(std::clamp(a, 0.0f, 255.0f));
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
}*/

Vertex packVertex(
    float x, float y, float z, float normalX, float normalY, float normalZ, float u, float v, uint32_t textureIndex)
{
    // clang-format off
    return Vertex{
        pack3x10(packUint10VertexCoordinate(x),
                packUint10VertexCoordinate(y),
                packUint10VertexCoordinate(z)),
        pack4x8(packSnorm8(normalX),
                packSnorm8(normalY),
                packSnorm8(normalZ),
                0),
        (float)textureIndex,
        pack4x8(packUint8(u),
                packUint8(v),
                0,
                0),
    };
    // clang-format on
}

VertexLod
packVertexLod(float x, float y, float z, float normalX, float normalY, float normalZ, float r, float g, float b)
{
    int normal = 0;
    if (std::abs(normalY) > std::abs(normalX) && std::abs(normalY) > std::abs(normalZ))
        normal = 2;
    if (std::abs(normalZ) > std::abs(normalX) && std::abs(normalZ) > std::abs(normalY))
        normal = 4;
    if (normal == 0 && normalX < 0)
        normal = 1;
    if (normal == 2 && normalY < 0)
        normal = 3;
    if (normal == 4 && normalZ < 0)
        normal = 5;
    // clang-format off
    return VertexLod(pack4x8(packUint8(x),
                             packUint8(y),
                             packUint8(z),
                             packUint8(normal)),

                     pack4x8(packUnorm8(r),
                             packUnorm8(g),
                             packUnorm8(b),
                             0));
    // clang-format on
}

void downsampleBlocks(const std::vector<BlockType>& blocks, int currentSize, std::vector<BlockType>& downsample)
{
    downsample.resize(currentSize * currentSize * currentSize / 8);
    for (int x = 0; x < currentSize; x += 2) {
        for (int y = 0; y < currentSize; y += 2) {
            for (int z = 0; z < currentSize; z += 2) {
                // TODO: implement properly
                BlockType block = blocks[x + y * currentSize + z * currentSize * currentSize];
                downsample[x / 2 + (y / 2) * currentSize / 2 + (z / 2) * currentSize / 2 * currentSize / 2] = block;
            }
        }
    }
}

void downsampleChunk(const Chunk& chunk, std::vector<BlockType>& downsample, int lod)
{
    assertm(lod <= 5, "Lod 5 is the max lod. Called with " << lod);

    if (!chunk.containsDifferentBlocks) {
        int size = 32 >> lod;
        downsample.resize(size * size * size);
        for (auto& a : downsample) {
            a = chunk.blocks[0];
        }
        return;
    }

    if (lod == 0) {
        downsample.resize(CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE);
        for (int i = 0; i < CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE; i++) {
            downsample[i] = chunk.blocks[i];
        }
        return;
    }

    downsampleBlocks(chunk.blocks, CHUNK_SIZE, downsample);

    for (int i = 1; i < lod; i++) {
        std::vector<BlockType> newBlocks;
        downsampleBlocks(downsample, CHUNK_SIZE >> i, newBlocks);
        downsample = newBlocks;
    }
}

void createChunkMeshFullDetail(WorldManager& worldManager, glm::i32vec3 chunkLocation, std::vector<Vertex>& vertices)
{
    std::array<BlockType, (CHUNK_SIZE + 2) * (CHUNK_SIZE + 2) * (CHUNK_SIZE + 2)> blocks =
        std::array<BlockType, (CHUNK_SIZE + 2) * (CHUNK_SIZE + 2) * (CHUNK_SIZE + 2)>();

    assertm(worldManager.chunks.contains(chunkLocation), "Chunk not found while generating lod");
    Chunk& chunk = worldManager.chunks.at(chunkLocation);

    Chunk& chunkPosX = worldManager.chunks.at(chunkLocation + glm::i32vec3{1, 0, 0});
    Chunk& chunkNegX = worldManager.chunks.at(chunkLocation + glm::i32vec3{-1, 0, 0});
    Chunk& chunkPosY = worldManager.chunks.at(chunkLocation + glm::i32vec3{0, 1, 0});
    Chunk& chunkNegY = worldManager.chunks.at(chunkLocation + glm::i32vec3{0, -1, 0});
    Chunk& chunkPosZ = worldManager.chunks.at(chunkLocation + glm::i32vec3{0, 0, 1});
    Chunk& chunkNegZ = worldManager.chunks.at(chunkLocation + glm::i32vec3{0, 0, -1});

    if (chunk.containsDifferentBlocks) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            for (int y = 0; y < CHUNK_SIZE; y++) {
                for (int x = 0; x < CHUNK_SIZE; x++) {
                    BlockType block = chunk.blocks[x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_SIZE];
                    blocks[(x + 1) + (y + 1) * (CHUNK_SIZE + 2) + (z + 1) * (CHUNK_SIZE + 2) * (CHUNK_SIZE + 2)] =
                        block;
                }
            }
        }
    }
    else {
        BlockType block = chunk.blocks[0];
        if (block == BlockType::air) {
            return;
        }
        for (int z = 0; z < CHUNK_SIZE; z++) {
            for (int y = 0; y < CHUNK_SIZE; y++) {
                for (int x = 0; x < CHUNK_SIZE; x++) {
                    blocks[(x + 1) + (y + 1) * (CHUNK_SIZE + 2) + (z + 1) * (CHUNK_SIZE + 2) * (CHUNK_SIZE + 2)] =
                        block;
                }
            }
        }
    }

    for (int y = 0; y < CHUNK_SIZE; y++) {
        for (int x = 0; x < CHUNK_SIZE; x++) {
            blocks[blockArrayLocToIndex(x + 1, y + 1, 0)] = chunkGetBlockAtLocation(x, y, CHUNK_SIZE - 1, chunkNegZ);
            blocks[blockArrayLocToIndex(x + 1, y + 1, CHUNK_SIZE + 1)] = chunkGetBlockAtLocation(x, y, 0, chunkPosZ);
        }
    }
    for (int z = 0; z < CHUNK_SIZE; z++) {
        for (int x = 0; x < CHUNK_SIZE; x++) {
            blocks[blockArrayLocToIndex(x + 1, 0, z + 1)] = chunkGetBlockAtLocation(x, CHUNK_SIZE - 1, z, chunkNegY);
            blocks[blockArrayLocToIndex(x + 1, CHUNK_SIZE + 1, z + 1)] = chunkGetBlockAtLocation(x, 0, z, chunkPosY);
        }
    }
    for (int z = 0; z < CHUNK_SIZE; z++) {
        for (int y = 0; y < CHUNK_SIZE; y++) {
            blocks[blockArrayLocToIndex(0, y + 1, z + 1)] = chunkGetBlockAtLocation(CHUNK_SIZE - 1, y, z, chunkNegX);
            blocks[blockArrayLocToIndex(CHUNK_SIZE + 1, y + 1, z + 1)] = chunkGetBlockAtLocation(0, y, z, chunkPosX);
        }
    }
    std::vector<MeshFace> faces = std::vector<MeshFace>();
    blockArrayMesher(blocks, faces);

    for (MeshFace& face : faces) {
        glm::vec3 dir1 = -face.location[0] + face.location[1];
        glm::vec3 dir2 = -face.location[0] + face.location[2];
        glm::vec3 norm = -glm::normalize(glm::cross(dir1, dir2));

        for (int i = 0; i < 4; i++) {
            // clang-format off
            assertm(face.uv[i].x == std::floor(face.uv[i].x) && face.uv[i].y == std::floor(face.uv[i].y), "uv not integer");
            vertices.push_back(packVertex(
                face.location[i].x, face.location[i].y, face.location[i].z,
                norm.x, norm.y, norm.z,
                face.uv[i].x, face.uv[i].y,
                face.textureLayer
            ));
            // clang-format on
        }
    }
}

void createChunkMeshLod(WorldManager& worldManager,
                        glm::i32vec3 chunkLocation,
                        std::vector<VertexLod>& vertices,
                        int lod)
{
    assertm(lod <= 5, "Lod is high. Maybe remove this check. Called with " << lod);

    int chunkLenght = 1 << lod;
    int blockLenght = CHUNK_SIZE >> lod;

    // TODO: Make better
    for (int cx = 0; cx < chunkLenght; cx++) {
        for (int cy = 0; cy < chunkLenght; cy++) {
            for (int cz = 0; cz < chunkLenght; cz++) {
                glm::i32vec3 lodChunkLoc = chunkLocation + glm::i32vec3(cx, cy, cz);
                assertm(worldManager.chunks.contains(lodChunkLoc), "Chunk not found while generating lod");
                Chunk& chunk = worldManager.chunks.at(lodChunkLoc);
                if (chunk.containsDifferentBlocks) {
                    goto cantSkipRendering;
                }
            }
        }
    }
    return;
cantSkipRendering:

    std::array<BlockType, (CHUNK_SIZE + 2) * (CHUNK_SIZE + 2) * (CHUNK_SIZE + 2)> blocks =
        std::array<BlockType, (CHUNK_SIZE + 2) * (CHUNK_SIZE + 2) * (CHUNK_SIZE + 2)>();
    for (int cx = 0; cx < chunkLenght; cx++) {
        for (int cy = 0; cy < chunkLenght; cy++) {
            for (int cz = 0; cz < chunkLenght; cz++) {

                glm::i32vec3 lodChunkLoc = chunkLocation + glm::i32vec3(cx, cy, cz);
                assertm(worldManager.chunks.contains(lodChunkLoc), "Chunk not found while generating lod");
                Chunk& chunk = worldManager.chunks.at(lodChunkLoc);

                std::vector<BlockType> downsample;
                downsampleChunk(chunk, downsample, lod);

                for (int z = 0; z < blockLenght; z++) {
                    for (int y = 0; y < blockLenght; y++) {
                        for (int x = 0; x < blockLenght; x++) {
                            int fx = blockLenght * cx + x + 1;
                            int fy = blockLenght * cy + y + 1;
                            int fz = blockLenght * cz + z + 1;
                            BlockType block = downsample[x + y * blockLenght + z * blockLenght * blockLenght];
                            blocks[fx + fy * (CHUNK_SIZE + 2) + fz * (CHUNK_SIZE + 2) * (CHUNK_SIZE + 2)] = block;
                        }
                    }
                }
            }
        }
    }
    /*for (int cx = 0; cx < chunkLenght; cx++) {
        for (int cy = 0; cy < chunkLenght; cy++) {
            glm::i32vec3 negChunkLoc = chunkLocation + glm::i32vec3(cx, cy, -1);
            assertm(worldManager.chunks.contains(negChunkLoc), "Chunk not found while generating lod");
            Chunk& chunkNeg = worldManager.chunks.at(negChunkLoc);
            for (int y = 0; y < blockLenght; y++) {
                for (int x = 0; x < blockLenght; x++) {
                    BlockType block = chunkGetBlockAtLocation(x, y, CHUNK_SIZE - 1, chunkNeg);

                    int fx = blockLenght * cx + x + 1;
                    int fy = blockLenght * cy + y + 1;
                    int fz = blockLenght * cz + z + 1;
                    BlockType block = downsample[x + y * blockLenght + z * blockLenght * blockLenght];
                    blocks[fx + fy * (CHUNK_SIZE + 2) + fz * (CHUNK_SIZE + 2) * (CHUNK_SIZE + 2)] = block;
                }
            }
        }
    }*/
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_SIZE; y++) {
            blocks[blockArrayLocToIndex(x + 1, y + 1, 0)] = BlockType::air;
            blocks[blockArrayLocToIndex(x + 1, y + 1, CHUNK_SIZE + 1)] = BlockType::air;
        }
    }
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            blocks[blockArrayLocToIndex(x + 1, 0, z + 1)] = BlockType::air;
            blocks[blockArrayLocToIndex(x + 1, CHUNK_SIZE + 1, z + 1)] = BlockType::air;
        }
    }
    for (int y = 0; y < CHUNK_SIZE; y++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            blocks[blockArrayLocToIndex(0, y + 1, z + 1)] = BlockType::air;
            blocks[blockArrayLocToIndex(CHUNK_SIZE + 1, y + 1, z + 1)] = BlockType::air;
        }
    }
    std::vector<MeshFace> faces = std::vector<MeshFace>();
    blockArrayMesher(blocks, faces);

    for (MeshFace& face : faces) {

        glm::vec3 dir1 = -face.location[0] + face.location[1];
        glm::vec3 dir2 = -face.location[0] + face.location[2];
        glm::vec3 norm = -glm::normalize(glm::cross(dir1, dir2));
        glm::vec3 color = blockImageColors[face.textureLayer];

        for (int i = 0; i < 4; i++) {
            // clang-format off
            vertices.push_back(packVertexLod(
                face.location[i].x, face.location[i].y, face.location[i].z,
                norm.x, norm.y, norm.z,
                color.r, color.g, color.b
            ));
            // clang-format on
        }
    }
}
