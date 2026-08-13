#include "chunkMesher.hpp"

#include <chrono>

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

VertexLod packVertexLod(
    float x, float y, float z, float normalX, float normalY, float normalZ, float r, float g, float b)
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
    return VertexLod(packA2R10G10B10_UNORM(x / (float)CHUNK_SIZE, y / (float)CHUNK_SIZE, z / (float)CHUNK_SIZE, 0),
                     packR8G8B8A8_UNORM(r, g, b, (float)normal));
}

void createChunkMesh(WorldManager& worldManager, glm::i32vec3 chunkLocation, std::vector<Vertex>& vertices)
{
    Chunk* chunk = &worldManager.chunks[chunkLocation];

    /*if (!chunk->containsDifferentBlocks && !isBlockSolid(chunk->blocks[0])) {
        return;
    }*/

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
    std::vector<MeshFace> faces = std::vector<MeshFace>();
    blockArrayMesher(blocks, faces);

    for (MeshFace& face : faces) {

        glm::vec3 dir1 = -face.location[0] + face.location[1];
        glm::vec3 dir2 = -face.location[0] + face.location[2];
        glm::vec3 norm = -glm::normalize(glm::cross(dir1, dir2));

        for (int i = 0; i < 4; i++) {
            // clang-format off
            vertices.push_back(packVertex(
                face.location[i].x, face.location[i].y, face.location[i].z,
                norm.x, norm.y, norm.z,
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

void downsampleBlocks(std::vector<BlockType>& blocks, int currentSize, std::vector<BlockType>& downsample)
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

void downsampleChunk(Chunk& chunk, std::vector<BlockType>& downsample, int lod)
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

    std::vector<BlockType>& blocks = chunk.blocks;
    for (int i = 0; i < lod; i++) {
        std::vector<BlockType> newBlocks;
        downsampleBlocks(blocks, CHUNK_SIZE >> i, newBlocks);
        blocks = newBlocks;
    }
    downsample = blocks;
}

void createChunkMeshLod(WorldManager& worldManager,
                        glm::i32vec3 chunkLocation,
                        std::vector<VertexLod>& vertices,
                        int lod)
{
    assertm(lod <= 5, "Lod is high. Maybe remove this check. Called with " << lod);

    auto debugStartWait = std::chrono::high_resolution_clock::now();

    std::array<BlockType, (CHUNK_SIZE + 2) * (CHUNK_SIZE + 2) * (CHUNK_SIZE + 2)> blocks =
        std::array<BlockType, (CHUNK_SIZE + 2) * (CHUNK_SIZE + 2) * (CHUNK_SIZE + 2)>();

    int chunkLenght = 1 << lod;
    int blockLenght = CHUNK_SIZE >> lod;
    for (int cx = 0; cx < chunkLenght; cx++) {
        for (int cy = 0; cy < chunkLenght; cy++) {
            for (int cz = 0; cz < chunkLenght; cz++) {

                glm::i32vec3 lodChunkLoc = chunkLocation + glm::i32vec3(cx, cy, cz);
                assertm(worldManager.chunks.contains(lodChunkLoc), "Chunk not found while generating lod");
                Chunk& chunk = worldManager.chunks.at(lodChunkLoc);

                std::vector<BlockType> downsample;
                downsampleChunk(chunk, downsample, lod);

                for (int x = 0; x < blockLenght; x++) {
                    for (int y = 0; y < blockLenght; y++) {
                        for (int z = 0; z < blockLenght; z++) {
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
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_SIZE; y++) {
            blocks[blockArrayLocToIndex(x + 1, y + 1, 0)] = BlockType::stone;
            blocks[blockArrayLocToIndex(x + 1, y + 1, CHUNK_SIZE + 1)] = BlockType::stone;
        }
    }
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            blocks[blockArrayLocToIndex(x + 1, 0, z + 1)] = BlockType::stone;
            blocks[blockArrayLocToIndex(x + 1, CHUNK_SIZE + 1, z + 1)] = BlockType::stone;
        }
    }
    for (int y = 0; y < CHUNK_SIZE; y++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            blocks[blockArrayLocToIndex(0, y + 1, z + 1)] = BlockType::stone;
            blocks[blockArrayLocToIndex(CHUNK_SIZE + 1, y + 1, z + 1)] = BlockType::stone;
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

    auto debugEndWait = std::chrono::high_resolution_clock::now();
    debugMenuGlobals.chunkGenTimeTotal +=
        std::chrono::duration<float, std::chrono::microseconds::period>(debugEndWait - debugStartWait).count();
    debugMenuGlobals.chunksGenerated += 1;
}
