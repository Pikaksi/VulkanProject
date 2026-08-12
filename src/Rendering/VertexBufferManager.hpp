#pragma once

#include <vector>

#include "Rendering/Vertex.hpp"
#include "VulkanRendering/VulkanTypes.hpp"
#include "Rendering/GPUMemoryBlock.hpp"
#include "3dRendering/QuadStripIndexBuffer.hpp"
#include "3dRendering/WorldVertexTracker.hpp"

struct VertexBufferManager
{
    VertexBufferManager() {}
    VertexBufferManager(VulkanCoreInfo& vulkanCoreInfo, VkCommandPool commandPool, uint64_t worldVertexBufferSize);

    uint64_t addVerticesToWorld(VulkanCoreInfo& vulkanCoreInfo,
                                VkCommandPool commandPool,
                                std::vector<Vertex>& vertices,
                                glm::ivec3 chunkLocation);
    uint64_t addVerticesToWorldLod(VulkanCoreInfo& vulkanCoreInfo,
                                   VkCommandPool commandPool,
                                   std::vector<VertexLod>& vertices,
                                   glm::ivec3 chunkLocation,
                                   int lod);
    void freeWorldVerticesMemory(uint64_t memoryBlockLocation);

    void getWorldGeometryForRendering(VkBuffer& vertexBuffer,
                                      std::vector<WorldDrawCallData>& vertexOffsets,
                                      VkBuffer& indexBuffer);

    void getUIGeometryForRendering(VkBuffer& vertexBuffer,
                                   std::vector<VkDeviceSize>& vertexOffsets,
                                   std::vector<uint32_t>& batchIndexCounts,
                                   VkBuffer& quadStripIndexBuffer);

    void cleanUp(VulkanCoreInfo& vulkanCoreInfo);

    GpuMemoryBlock* worldGpuMemoryBlock;
    WorldVertexTracker worldVertexTracker;

    QuadStripIndexBuffer quadStripIndexBuffer;
};
