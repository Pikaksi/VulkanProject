#include <iostream>
#include <chrono>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>

#include "World/WorldManager.hpp"
#include "ChunkRenderer.hpp"
#include "assertm.hpp"
#include "chunkMesher.hpp"
#include "DebugMenu.hpp"
#include "vec3hash.hpp"

const int32_t firstBits[] = {0b0,
                             0b1,
                             0b11,
                             0b111,
                             0b1111,
                             0b11111,
                             0b111111,
                             0b1111111,
                             0b11111111,
                             0b111111111,
                             0b1111111111,
                             0b11111111111,
                             0b111111111111,
                             0b1111111111111,
                             0b11111111111111,
                             0b111111111111111,
                             0b1111111111111111,
                             0b11111111111111111};
glm::i32vec3 roundLocationLod(glm::i32vec3 loc, int lod)
{
    loc.x = loc.x & (~firstBits[lod]);
    loc.y = loc.y & (~firstBits[lod]);
    loc.z = loc.z & (~firstBits[lod]);
    return loc;
}

float chunkDistanceOriginSquared(glm::i32vec3 loc, int lod)
{
    glm::f32vec3 center = (glm::f32vec3)loc + (float)(1 << std::max(lod, 0)) / 2.0f - 0.5f;
    return center.x * center.x + center.y * center.y + center.z * center.z;
}

void generateRenderingOrder(std::vector<ChunkOrderInfo>& offsets, std::vector<int>& renderDistances)
{
    /*offsets = {
        ChunkOrderInfo{{0, 0, 0}, 0, true},
        ChunkOrderInfo{{0, 0, 1}, 0, true},
        ChunkOrderInfo{{0, 1, 0}, 0, true},
        ChunkOrderInfo{{0, 1, 1}, 0, true},
        ChunkOrderInfo{{1, 0, 0}, 0, true},
        ChunkOrderInfo{{1, 0, 1}, 0, true},
        ChunkOrderInfo{{1, 1, 0}, 0, true},
        ChunkOrderInfo{{1, 1, 1}, 0, true},
    };
    return;*/
    for (int i = 0; i < renderDistances.size() - 1; i++) {
        assertm(renderDistances[i] <= renderDistances[i + 1],
                "lod " << i << " and lod " << (i + 1) << " renderdistances overlap badly.");
    }

    int maxDistance = renderDistances.back();
    int maxLod = renderDistances.size() - 2;
    int maxSize = 1 << maxLod;
    for (int x = -maxDistance - maxSize; x <= maxDistance + maxSize; x += maxSize) {
        for (int y = -maxDistance - maxSize; y <= maxDistance + maxSize; y += maxSize) {
            for (int z = -maxDistance - maxSize; z <= maxDistance + maxSize; z += maxSize) {
                if (x * x + y * y + z * z > maxDistance * maxDistance)
                    continue;
                offsets.push_back(
                    ChunkOrderInfo{.loc = roundLocationLod(glm::i32vec3{x, y, z}, maxLod), .lod = maxLod});
            }
        }
    }

    for (int newLod = renderDistances.size() - 2; newLod >= 0; newLod--) {
        int distance = renderDistances[newLod];
        int chunksAtStart = offsets.size();
        for (int k = 0; k < chunksAtStart; k++) {
            ChunkOrderInfo& chunk = offsets[k];
            // should have been decomposed last iteration
            if (chunk.lod != newLod + 1)
                continue;
            float centerDistance = chunkDistanceOriginSquared(chunk.loc, chunk.lod);
            if (centerDistance > distance * distance)
                continue;

            chunk.lod = newLod;
            const glm::i32vec3 cubeOffsets[] = {
                glm::i32vec3{0, 0, 1},
                glm::i32vec3{0, 1, 0},
                glm::i32vec3{0, 1, 1},
                glm::i32vec3{1, 0, 0},
                glm::i32vec3{1, 0, 1},
                glm::i32vec3{1, 1, 0},
                glm::i32vec3{1, 1, 1},
            };
            for (glm::i32vec3 a : cubeOffsets) {
                offsets.push_back(ChunkOrderInfo{.loc = chunk.loc + a * (1 << newLod), .lod = newLod});
            }
        }
    }

    std::sort(offsets.begin(), offsets.end(), [](const ChunkOrderInfo& a, const ChunkOrderInfo& b) {
        return chunkDistanceOriginSquared(a.loc, a.lod) < chunkDistanceOriginSquared(b.loc, b.lod);
    });

    for (int i = 0; i < offsets.size(); i++) {
        ChunkOrderInfo& chunk = offsets[i];
        if (chunkDistanceOriginSquared(chunk.loc, chunk.lod) > renderDistances[0] * renderDistances[0])
            break;

        chunk.fullDetail = true;
    }

    std::unordered_set<glm::i32vec3, Vec3LocalizedHash> assertSet;
    for (int i = 0; i < offsets.size(); i++) {
        ChunkOrderInfo& chunk = offsets[i];

        assertm(!assertSet.contains(chunk.loc), "Duplicate chunk in generation order");
        assertSet.insert(chunk.loc);

        assertm(chunk.lod >= 0, "Lod below zero in generation");
        /*std::cout << "chunk at " << offsets[i].loc.x << " " << offsets[i].loc.y << " " << offsets[i].loc.z
                  << "  lod = " << offsets[i].lod
                  << "  distance from center = " << chunkDistanceOriginSquared(chunk.loc, chunk.lod) << std::endl;*/
    }
}

ChunkRenderer::ChunkRenderer() { generateRenderingOrder(chunkRenderingOrder, renderDistances); }

ChunkRenderingCommand ChunkRenderer::getNextChunkToRender(glm::i32vec3 playerLocation)
{
    /*{
        for (; nextChunkRenderIndex < chunkRenderingOrder.size(); nextChunkRenderIndex++) {
            ChunkRenderingCommand command;
            command.noChunksToRender = false;
            command.replace = false;
            command.loc = playerLocation + chunkRenderingOrder[nextChunkRenderIndex].loc;
            command.toLod = chunkRenderingOrder[nextChunkRenderIndex].lod;
            command.fullDetail = chunkRenderingOrder[nextChunkRenderIndex].fullDetail;
            nextChunkRenderIndex++;
            return command;
        }
        ChunkRenderingCommand command;
        command.noChunksToRender = true;
        return command;
    }*/

    for (; nextChunkRenderIndex < chunkRenderingOrder.size(); nextChunkRenderIndex++) {
        ChunkRenderingCommand command;
        command.noChunksToRender = false;
        command.replace = false;
        command.loc = playerLocation + chunkRenderingOrder[nextChunkRenderIndex].loc;
        command.toLod = chunkRenderingOrder[nextChunkRenderIndex].lod;
        command.fullDetail = chunkRenderingOrder[nextChunkRenderIndex].fullDetail;

        bool foundChunk = false;
        int existingLod = 0;
        for (; existingLod <= maxLod; existingLod++) {
            if (chunkInfos.contains(roundLocationLod(command.loc, existingLod))) {
                foundChunk = true;
                break;
            }
        }
        // Chunk already exists
        if (foundChunk && existingLod == command.toLod) {
            continue;
        }
        // render a new chunk
        if (!foundChunk) {
            nextChunkRenderIndex++;
            return command;
        }
        // replace old chunk
        command.fromLod = existingLod;
        command.replace = true;
        nextChunkRenderIndex++;
        return command;
    }
    ChunkRenderingCommand command;
    command.noChunksToRender = true;
    return command;
}

void ChunkRenderer::handleRenderCommand(VulkanCoreInfo& vulkanCoreInfo,
                                        VkCommandPool commandPool,
                                        WorldManager& worldManager,
                                        VertexBufferManager& vertexBufferManager,
                                        ChunkRenderingCommand& renderCommand)
{
    assertm(renderCommand.noChunksToRender == false, "not allowed");
    assertm(renderCommand.toLod >= 0, "Lod below zero");

    /*std::cout << "Handling command with\n  loc: " << renderCommand.loc.x << " " << renderCommand.loc.y << " "
              << renderCommand.loc.z << "\n  toLod: " << renderCommand.toLod
              << "\n  fullDetai: " << renderCommand.fullDetail << "\n  replace: " << renderCommand.replace
              << "\n  fromLod: " << renderCommand.fromLod << std::endl;*/

    if (renderCommand.replace == true)
        return;

    // TODO: Does not handle resizing chunk to multiple different sizes
    if (renderCommand.replace) {
        assertm(renderCommand.toLod != renderCommand.fromLod, "Replacing chunk with the same lod");
        // replace smaller chunks with a large one
        if (false) {
            if (renderCommand.toLod > renderCommand.fromLod) {
                int chunkSize = 1 << renderCommand.toLod;
                for (int x = 0; x < chunkSize; x++) {
                    for (int y = 0; y < chunkSize; y++) {
                        for (int z = 0; z < chunkSize; z++) {
                            glm::i32vec3 loc =
                                roundLocationLod(renderCommand.loc, renderCommand.toLod) + glm::i32vec3{x, y, z};
                            if (chunkInfos.contains(loc)) {
                                uint64_t memoryLoc = chunkInfos.at(loc).memoryLocation;
                                vertexBufferManager.freeWorldVerticesMemory(memoryLoc);
                            }
                        }
                    }
                }
            }
        }
        // replace a large chunk with smaller ones
        else {
            if (false) {
                glm::i32vec3 largeChunkLoc = roundLocationLod(renderCommand.loc, renderCommand.fromLod);
                uint64_t memoryLoc = chunkInfos.at(largeChunkLoc).memoryLocation;
                vertexBufferManager.freeWorldVerticesMemory(memoryLoc);
            }

            int chunks = (1 << renderCommand.fromLod) >> renderCommand.toLod;
            for (int x = 0; x < chunks; x++) {
                for (int y = 0; y < chunks; y++) {
                    for (int z = 0; z < chunks; z++) {
                        glm::i32vec3 loc = roundLocationLod(renderCommand.loc, renderCommand.fromLod) +
                                           glm::i32vec3{x, y, z} * (1 << renderCommand.toLod);
                        renderChunk2(vulkanCoreInfo,
                                     commandPool,
                                     worldManager,
                                     vertexBufferManager,
                                     loc,
                                     renderCommand.toLod,
                                     renderCommand.fullDetail);
                    }
                }
            }
            return;
        }
    }
    renderChunk2(vulkanCoreInfo,
                 commandPool,
                 worldManager,
                 vertexBufferManager,
                 renderCommand.loc,
                 renderCommand.toLod,
                 renderCommand.fullDetail);
}

void ChunkRenderer::renderChunk2(VulkanCoreInfo& vulkanCoreInfo,
                                 VkCommandPool commandPool,
                                 WorldManager& worldManager,
                                 VertexBufferManager& vertexBufferManager,
                                 glm::i32vec3 loc,
                                 int lod,
                                 bool fullDetail)
{
    int size = 1 << lod;
    // TODO: no corner chunks
    for (int x = -1; x < size + 1; x++) {
        for (int y = -1; y < size + 1; y++) {
            for (int z = -1; z < size + 1; z++) {
                glm::i32vec3 newLoc = loc + glm::i32vec3{x, y, z};
                worldManager.tryGeneratingNewChunk(newLoc, chunksToRenderAgain, *this);
            }
        }
    }

    /*for (glm::i32vec3 chunk : chunksToRenderAgain) {
        if (chunk.x == loc.x && chunk.y == loc.y && chunk.z == loc.z) {
            tryAddChunkToRender(chunk);
        }
    }*/


    std::cout << "         actually rendering chunk, loc: " << loc.x << " " << loc.y << " " << loc.z << ", lod: " << lod
              << std::endl;
    uint64_t memoryBlockPointer = -1;
    if (fullDetail) {
        std::vector<Vertex> vertices;
        createChunkMesh(worldManager, loc, vertices);
        if (vertices.size() == 0) {
            return;
        }
        memoryBlockPointer = vertexBufferManager.addVerticesToWorld(vulkanCoreInfo, commandPool, vertices, loc);
    }
    else {
        std::vector<VertexLod> vertices;
        createChunkMeshLod(worldManager, loc, vertices, lod);
        if (vertices.size() == 0) {
            return;
        }
        memoryBlockPointer = vertexBufferManager.addVerticesToWorldLod(vulkanCoreInfo, commandPool, vertices, loc, lod);
    }

    chunkInfos.insert(std::make_pair(loc, memoryBlockPointer));
}

void ChunkRenderer::renderNextChunk(VulkanCoreInfo& vulkanCoreInfo,
                                    VkCommandPool commandPool,
                                    WorldManager& worldManager,
                                    VertexBufferManager& vertexBufferManager,
                                    glm::i32vec3 playerChunkLocation)
{
    ChunkRenderingCommand renderCommand = getNextChunkToRender(playerChunkLocation);
    if (renderCommand.noChunksToRender) {
        return;
    }

    handleRenderCommand(vulkanCoreInfo, commandPool, worldManager, vertexBufferManager, renderCommand);
}

void ChunkRenderer::update(VulkanCoreInfo& vulkanCoreInfo,
                           VkCommandPool commandPool,
                           WorldManager& worldManager,
                           VertexBufferManager& vertexBufferManager,
                           glm::i32vec3 playerChunkLocation)
{
    // Render some chunks again
    /*for (int i = 0; i < chunksToRenderAgain.size(); i++) {
        glm::i32vec3 loc = chunksToRenderAgain[i];
        if (!chunkInfos.contains(loc))
            continue;

        ChunkInfo chunkInfo = chunkInfos.at(loc);
        vertexBufferManager.freeWorldVerticesMemory(chunkInfo.memoryLocation);
        chunkInfos.erase(loc);
        renderChunk2(vulkanCoreInfo, commandPool, worldManager, vertexBufferManager, loc, chunkInfo.lod);
    }
    chunksToRenderAgain.clear();*/

    // Don't rerender chunks rendering position did not move.
    if (peviousPlayerChunkLocation != playerChunkLocation) {
        nextChunkRenderIndex = 0;
    }
    peviousPlayerChunkLocation = playerChunkLocation;

    static int counter = 0;
    counter++;
    if (counter >= 10) {
        counter = 0;
        renderNextChunk(vulkanCoreInfo, commandPool, worldManager, vertexBufferManager, playerChunkLocation);
    }

    // TODO: Derender chunks
    // derenderChunksOutOfRenderdistance(playerChunkLocation, vertexBufferManager);
}

void ChunkRenderer::rerenderChunk(glm::i32vec3 chunkLocation)
{
    if (!chunkInfos.contains(chunkLocation)) {
        return;
    }
    for (size_t i = 0; i < chunksToRenderAgain.size(); i++) {
        if (chunksToRenderAgain[i] == chunkLocation) {
            return;
        }
    }
    chunksToRenderAgain.push_back(chunkLocation);
}

// ----------------- OLD VERSION -----------------

/*void ChunkRenderer::renderNewChunksInRenderdistance(glm::i32vec3 playerChunkLocation, int newChunkMinDistance)
{
    int cubesChecked = 0;
    // std::cout << "min distance for new chunk = " << newChunkMinDistance << "\n";

    // Render close by chunks first.
    // Checks a hollow box for each distance starting from the smallest.
    for (int distance = newChunkMinDistance; distance < renderDistance; distance++) {
        // x direction with all edges
        for (int z = -distance; z <= distance; z++) {
            for (int y = -distance; y <= distance; y++) {

                cubesChecked += 2;
                tryAddChunkToRender(glm::i32vec3(distance, y, z) + playerChunkLocation);
                tryAddChunkToRender(glm::i32vec3(-distance, y, z) + playerChunkLocation);
            }
        }

        // z direction with not x axis edges
        for (int x = -distance + 1; x <= distance - 1; x++) {
            for (int y = -distance; y <= distance; y++) {

                cubesChecked += 2;
                tryAddChunkToRender(glm::i32vec3(x, y, distance) + playerChunkLocation);
                tryAddChunkToRender(glm::i32vec3(x, y, -distance) + playerChunkLocation);
            }
        }

        // y direction with no edges
        for (int x = -distance + 1; x <= distance - 1; x++) {
            for (int z = -distance + 1; z <= distance - 1; z++) {

                cubesChecked += 2;
                tryAddChunkToRender(glm::i32vec3(x, distance, z) + playerChunkLocation);
                tryAddChunkToRender(glm::i32vec3(x, -distance, z) + playerChunkLocation);
            }
        }
    }
}

void ChunkRenderer::derenderChunksOutOfRenderdistance(glm::i32vec3 playerChunkLocation,
                                                      VertexBufferManager& vertexBufferManager)
{
    int minX = playerChunkLocation.x - renderDistance - extraRangeToDerenderChunk;
    int maxX = playerChunkLocation.x + renderDistance + extraRangeToDerenderChunk;
    int minY = playerChunkLocation.y - renderDistance - extraRangeToDerenderChunk;
    int maxY = playerChunkLocation.y + renderDistance + extraRangeToDerenderChunk;
    int minZ = playerChunkLocation.z - renderDistance - extraRangeToDerenderChunk;
    int maxZ = playerChunkLocation.z + renderDistance + extraRangeToDerenderChunk;

    std::vector<glm::i32vec3> chunksToDerender;

    for (const auto& pair : chunkInfos) {
        if (pair.first.x <= minX || pair.first.x >= maxX || pair.first.y <= minY || pair.first.y >= maxY ||
            pair.first.z <= minZ || pair.first.z >= maxZ) {
            // std::cout << "derendered chunk at " << pair.first.x << " " << pair.first.y << " " << pair.first.z <<
            // "\n";
            vertexBufferManager.freeWorldVerticesMemory(pair.second);
            chunksToDerender.push_back(pair.first);
        }
    }

    for (const auto& chunkLocation : chunksToDerender) {
        chunkInfos.erase(chunkLocation);
    }
}

void ChunkRenderer::tryAddChunkToRender(glm::i32vec3 chunkLocation)
{
    if (!chunkInfos.contains(chunkLocation)) {
        chunksToRender.push(chunkLocation);
    }
}

bool ChunkRenderer::chunkIsInRenderDistance(glm::i32vec3 playerChunkLocation, glm::i32vec3 chunkLocation)
{
    int minX = playerChunkLocation.x - renderDistance;
    int maxX = playerChunkLocation.x + renderDistance;
    int minY = playerChunkLocation.y - renderDistance;
    int maxY = playerChunkLocation.y + renderDistance;
    int minZ = playerChunkLocation.z - renderDistance;
    int maxZ = playerChunkLocation.z + renderDistance;

    return chunkLocation.x >= minX && chunkLocation.x <= maxX && chunkLocation.y >= minY && chunkLocation.y <= maxY &&
           chunkLocation.z >= minZ && chunkLocation.z <= maxZ;
}

void ChunkRenderer::addQueuedChunkMeshes(VulkanCoreInfo& vulkanCoreInfo,
                                         VkCommandPool commandPool,
                                         WorldManager& worldManager,
                                         VertexBufferManager& vertexBufferManager,
                                         glm::i32vec3 playerChunkLocation)
{
    for (int i = 0; i < chunksToRenderAgain.size(); i++) {
        glm::i32vec3 chunkLocation = chunksToRenderAgain[i];
        if (chunkIsInRenderDistance(playerChunkLocation, chunkLocation)) {
            if (chunkInfos.contains(chunkLocation)) {
                uint32_t memoryLocation = chunkInfos.at(chunkLocation);
                vertexBufferManager.freeWorldVerticesMemory(memoryLocation);
                chunkInfos.erase(chunkLocation);
            }
            renderChunk(vulkanCoreInfo, commandPool, chunkLocation, worldManager, vertexBufferManager);
        }
    }
    chunksToRenderAgain.clear();

    if (chunksToRender.size() == 0) {
        return;
    }
    glm::i32vec3 chunkLocation = chunksToRender.front();
    chunksToRender.pop();

    if (chunkInfos.contains(chunkLocation)) {
        return;
    }

    if (chunkIsInRenderDistance(playerChunkLocation, chunkLocation)) {
        renderChunk(vulkanCoreInfo, commandPool, chunkLocation, worldManager, vertexBufferManager);
    }
}

void ChunkRenderer::renderChunk(VulkanCoreInfo& vulkanCoreInfo,
                                VkCommandPool commandPool,
                                glm::i32vec3 chunkLocation,
                                WorldManager& worldManager,
                                VertexBufferManager& vertexBufferManager)
{
    std::vector<glm::ivec3> chunksToRerender;
    worldManager.tryGeneratingNewChunk(chunkLocation, chunksToRerender, *this);
    // generate adjacent chunks so that we dont have to rerender this chunk when they are generated
    chunkLocation.x -= 1;
    worldManager.tryGeneratingNewChunk(chunkLocation, chunksToRerender, *this);
    chunkLocation.x += 2;
    worldManager.tryGeneratingNewChunk(chunkLocation, chunksToRerender, *this);
    chunkLocation.x -= 1;

    chunkLocation.y -= 1;
    worldManager.tryGeneratingNewChunk(chunkLocation, chunksToRerender, *this);
    chunkLocation.y += 2;
    worldManager.tryGeneratingNewChunk(chunkLocation, chunksToRerender, *this);
    chunkLocation.y -= 1;

    chunkLocation.z -= 1;
    worldManager.tryGeneratingNewChunk(chunkLocation, chunksToRerender, *this);
    chunkLocation.z += 2;
    worldManager.tryGeneratingNewChunk(chunkLocation, chunksToRerender, *this);
    chunkLocation.z -= 1;

    for (auto chunk : chunksToRerender) {
        if (chunk.x == chunkLocation.x && chunk.y == chunkLocation.y && chunk.z == chunkLocation.z) {
            tryAddChunkToRender(chunk);
        }
    }

    std::vector<Vertex> vertices;
    createChunkMesh(worldManager, chunkLocation, vertices);

    if (vertices.size() == 0) {
        return;
    }
    uint64_t memoryBlockPointer =
        vertexBufferManager.addVerticesToWorld(vulkanCoreInfo, commandPool, vertices, chunkLocation);
    chunkInfos.insert(std::make_pair(chunkLocation, memoryBlockPointer));
}*/
