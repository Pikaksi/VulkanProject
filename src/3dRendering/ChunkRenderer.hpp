#pragma once

#include <vector>
#include <unordered_map>
#include <queue>

struct WorldManager;

#include "VulkanRendering/VulkanTypes.hpp"
#include "Rendering/VertexBufferManager.hpp"
#include "util/vec3hash.hpp"

struct ChunkOrderInfo
{
    glm::i32vec3 loc;
    int lod;
    bool fullDetail;
};

struct ChunkInfo
{
    int lod;
    uint64_t memoryLocation;
    bool fullDetail;
};

struct ChunkRenderingCommand
{
    glm::i32vec3 loc;
    int fromLod;
    int toLod;
    bool replace;
    bool noChunksToRender;
    bool fullDetail;
};

glm::i32vec3 roundLocationLod(glm::i32vec3 loc, int lod);

struct ChunkRenderer
{
    ChunkRenderer();

    std::vector<ChunkOrderInfo> chunkRenderingOrder;

    const int renderDistancelodFull = 8;
    const int renderDistancelod0 = 16;
    const int renderDistancelod1 = 32;
    const int renderDistancelod2 = 64;
    const int renderDistancelod3 = 128;
    std::vector<int> renderDistances = {
        renderDistancelodFull, renderDistancelod0, renderDistancelod1, renderDistancelod2, renderDistancelod3};
    const int extraRangeToDerenderChunk = 0;

    int nextChunkRenderIndex = 0;
    int nextChunkGenerationIndex = 0;
    std::vector<glm::i32vec3> chunksToRenderAgain;
    std::unordered_map<glm::i32vec3, ChunkInfo, Vec3LocalizedHash> chunkInfos;
    glm::i32vec3 peviousPlayerChunkLocation = glm::i32vec3(9999, 9999, 9999);

    // void derenderChunksOutOfRenderdistance(glm::i32vec3 playerChunkLocation, VertexBufferManager&
    // vertexBufferManager); void renderNewChunksInRenderdistance(glm::i32vec3 playerChunkLocation, int
    // newChunkMinDistance); void tryAddChunkToRender(glm::i32vec3 chunkLocation); bool
    // chunkIsInRenderDistance(glm::i32vec3 playerChunkLocation, glm::i32vec3 chunkLocation);
    /*void addQueuedChunkMeshes(VulkanCoreInfo& vulkanCoreInfo,
                              VkCommandPool commandPool,
                              WorldManager& worldManager,
                              VertexBufferManager& vertexBufferManager,
                              glm::i32vec3 playerChunkLocation);*/
    /*void renderChunk(VulkanCoreInfo& vulkanCoreInfo,
                     VkCommandPool commandPool,
                     glm::i32vec3 chunkLocation,
                     WorldManager& worldManager,
                     VertexBufferManager& vertexBufferManager);*/
    void update(VulkanCoreInfo& vulkanCoreInfo,
                VkCommandPool commandPool,
                WorldManager& worldManager,
                VertexBufferManager& vertexBufferManager,
                glm::i32vec3 playerChunkLocation);
    void rerenderChunkAgain(glm::i32vec3 chunkLocation);
    ChunkRenderingCommand getNextChunkToRender(glm::i32vec3 playerLocation, int& orderIndex);
    void renderChunk(VulkanCoreInfo& vulkanCoreInfo,
                     VkCommandPool commandPool,
                     WorldManager& worldManager,
                     VertexBufferManager& vertexBufferManager,
                     glm::i32vec3 loc,
                     int lod,
                     bool fullDetail);

    void handleRenderCommand(VulkanCoreInfo& vulkanCoreInfo,
                             VkCommandPool commandPool,
                             WorldManager& worldManager,
                             VertexBufferManager& vertexBufferManager,
                             ChunkRenderingCommand& renderCommand);
    void queueGenerationOfNewChunks(VulkanCoreInfo& vulkanCoreInfo,
                                    VkCommandPool commandPool,
                                    WorldManager& worldManager,
                                    VertexBufferManager& vertexBufferManager,
                                    glm::i32vec3 playerChunkLocation);
    void updateRenderCommands(VulkanCoreInfo& vulkanCoreInfo,
                              VkCommandPool commandPool,
                              WorldManager& worldManager,
                              VertexBufferManager& vertexBufferManager,
                              glm::i32vec3 playerLocation);
};
