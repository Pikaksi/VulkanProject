#include <iostream>
#include <chrono>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>

#include "World/WorldManager.hpp"
#include "ChunkRenderer.hpp"
#include "assertm.hpp"
#include "chunkMesher.hpp"
#include "vec3hash.hpp"
#include "DebugMenu.hpp"

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
    assertm(lod <= 10, "Lod too large for rounding implementation. Lod = " << lod);
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

void generateRenderingOrder(std::vector<ChunkOrderInfo>& offsets,
                            std::vector<int>& renderDistances,
                            glm::i32vec3 playerLoc)
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
                    ChunkOrderInfo{.loc = roundLocationLod(glm::i32vec3{x, y, z} + playerLoc, maxLod), .lod = maxLod});
            }
        }
    }

    for (int newLod = renderDistances.size() - 2; newLod >= 0; newLod--) {
        int distance = renderDistances[newLod + 1];
        int chunksAtStart = offsets.size();
        for (int k = 0; k < chunksAtStart; k++) {
            ChunkOrderInfo& chunk = offsets[k];
            // should have been decomposed last iteration
            if (chunk.lod != newLod + 1)
                continue;
            float centerDistance = chunkDistanceOriginSquared(chunk.loc - playerLoc, chunk.lod);
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

    std::sort(offsets.begin(), offsets.end(), [playerLoc](const ChunkOrderInfo& a, const ChunkOrderInfo& b) {
        return chunkDistanceOriginSquared(a.loc - playerLoc, a.lod) <
               chunkDistanceOriginSquared(b.loc - playerLoc, b.lod);
    });

    for (int i = 0; i < offsets.size(); i++) {
        ChunkOrderInfo& chunk = offsets[i];
        if (chunkDistanceOriginSquared(chunk.loc - playerLoc, chunk.lod) > renderDistances[0] * renderDistances[0])
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
                  << "  distance from center = " << chunkDistanceOriginSquared(chunk.loc - playerLoc, chunk.lod) <<
           std::endl;*/
    }
}

ChunkRenderer::ChunkRenderer() {}

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
        command.loc = chunkRenderingOrder[nextChunkRenderIndex].loc;
        command.toLod = chunkRenderingOrder[nextChunkRenderIndex].lod;
        command.fullDetail = chunkRenderingOrder[nextChunkRenderIndex].fullDetail;

        bool foundChunk = false;
        int existingLod = 0;
        ChunkInfo* chunkInfo;
        for (; existingLod <= renderDistances.size(); existingLod++) {
            if (chunkInfos.contains(roundLocationLod(command.loc, existingLod))) {
                chunkInfo = &chunkInfos.at(roundLocationLod(command.loc, existingLod));
                if (chunkInfo->lod < existingLod)
                    continue;

                foundChunk = true;
                break;
            }
        }
        // render a new chunk
        if (!foundChunk) {
            command.replace = false;
            nextChunkRenderIndex++;
            return command;
        }
        // check if We are changing ful detail to lod with same size
        if (foundChunk && existingLod == 0 && command.toLod == 0 && chunkInfo->fullDetail != command.fullDetail) {
            command.fromLod = 0;
            command.replace = true;
            nextChunkRenderIndex++;
            return command;
        }
        // Chunk already exists
        if (foundChunk && existingLod == command.toLod) {
            continue;
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

    // TODO: Does not handle resizing chunk to multiple different sizes
    if (renderCommand.replace) {
        assertm(renderCommand.toLod != renderCommand.fromLod || renderCommand.toLod == 0,
                "Replacing chunk with the same lod");
        // replace smaller chunks with a large one
        if (renderCommand.toLod > renderCommand.fromLod) {
            int chunkSize = 1 << renderCommand.fromLod;

            for (int x = 0; x < chunkSize; x++) {
                for (int y = 0; y < chunkSize; y++) {
                    for (int z = 0; z < chunkSize; z++) {
                        glm::i32vec3 loc =
                            roundLocationLod(renderCommand.loc, renderCommand.fromLod) + glm::i32vec3{x, y, z};
                        if (chunkInfos.contains(loc)) {
                            uint64_t memoryLoc = chunkInfos.at(loc).memoryLocation;
                            chunkInfos.erase(loc);
                            vertexBufferManager.freeWorldVerticesMemory(memoryLoc);
                        }
                    }
                }
            }
        }
        // replace a large chunk with smaller ones
        else {
            glm::i32vec3 largeChunkLoc = roundLocationLod(renderCommand.loc, renderCommand.fromLod);

            assertm(chunkInfos.contains(largeChunkLoc), "Chunk was not found when replacing one");
            uint64_t memoryLoc = chunkInfos.at(largeChunkLoc).memoryLocation;
            chunkInfos.erase(largeChunkLoc);
            vertexBufferManager.freeWorldVerticesMemory(memoryLoc);

            int chunks = (1 << renderCommand.fromLod) >> renderCommand.toLod;
            for (int x = 0; x < chunks; x++) {
                for (int y = 0; y < chunks; y++) {
                    for (int z = 0; z < chunks; z++) {
                        glm::i32vec3 loc = roundLocationLod(renderCommand.loc, renderCommand.fromLod) +
                                           glm::i32vec3{x, y, z} * (1 << renderCommand.toLod);
                        renderChunk(vulkanCoreInfo,
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
    renderChunk(vulkanCoreInfo,
                commandPool,
                worldManager,
                vertexBufferManager,
                renderCommand.loc,
                renderCommand.toLod,
                renderCommand.fullDetail);
}

void ChunkRenderer::renderChunk(VulkanCoreInfo& vulkanCoreInfo,
                                VkCommandPool commandPool,
                                WorldManager& worldManager,
                                VertexBufferManager& vertexBufferManager,
                                glm::i32vec3 loc,
                                int lod,
                                bool fullDetail)
{
    assertm(roundLocationLod(loc, lod) == loc, "Chunk location not rounded properly to grid with lod");

    int size = 1 << lod;
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

    uint64_t memoryBlockPointer = -1;
    if (fullDetail) {
        std::vector<Vertex> vertices;

        auto debugStartWait = std::chrono::high_resolution_clock::now();

        createChunkMesh(worldManager, loc, vertices);

        auto debugEndWait = std::chrono::high_resolution_clock::now();
        debugMenuGlobals.chunkGenTimeTotal +=
            std::chrono::duration<float, std::chrono::microseconds::period>(debugEndWait - debugStartWait).count();

        if (vertices.size() == 0) {
            return;
        }
        memoryBlockPointer = vertexBufferManager.addVerticesToWorld(vulkanCoreInfo, commandPool, vertices, loc);
    }
    else {
        std::vector<VertexLod> vertices;

        auto debugStartWait = std::chrono::high_resolution_clock::now();

        createChunkMeshLod(worldManager, loc, vertices, lod);

        auto debugEndWait = std::chrono::high_resolution_clock::now();
        debugMenuGlobals.chunkGenTimeTotal +=
            std::chrono::duration<float, std::chrono::microseconds::period>(debugEndWait - debugStartWait).count();

        if (vertices.size() == 0) {
            return;
        }
        memoryBlockPointer = vertexBufferManager.addVerticesToWorldLod(vulkanCoreInfo, commandPool, vertices, loc, lod);
    }

    assertm(!chunkInfos.contains(loc), "Tried to place duplicate memory location for a chunk location");
    chunkInfos.insert(
        std::make_pair(loc, ChunkInfo{.memoryLocation = memoryBlockPointer, .fullDetail = fullDetail, .lod = lod}));
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
        chunkRenderingOrder.clear();
        generateRenderingOrder(chunkRenderingOrder, renderDistances, playerChunkLocation);
    }
    peviousPlayerChunkLocation = playerChunkLocation;

    static int counter = 0;
    counter++;
    if (counter >= 1) {
        counter = 0;
        renderNextChunk(vulkanCoreInfo, commandPool, worldManager, vertexBufferManager, playerChunkLocation);
    }

    // TODO: Derender chunks
    // derenderChunksOutOfRenderdistance(playerChunkLocation, vertexBufferManager);
}

void ChunkRenderer::rerenderChunkAgain(glm::i32vec3 chunkLocation)
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
