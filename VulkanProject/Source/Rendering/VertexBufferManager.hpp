#pragma once

#include <vector>

#include "Rendering/Vertex.hpp"
#include "VulkanRendering/VulkanTypes.hpp"
#include "VulkanRendering/Buffers.hpp"
#include "Rendering/GPUMemoryBlock.hpp"
#include "3dRendering/QuadStripIndexBuffer.hpp"
#include "3dRendering/WorldVertexTracker.hpp"

class VertexBufferManager
{
public:
    VertexBufferManager() {}
    VertexBufferManager(VulkanCoreInfo& vulkanCoreInfo, VkCommandPool commandPool, uint32_t worldVertexCount, uint32_t uiMaxVertexCount);

    uint32_t addVerticesToWorld(VulkanCoreInfo& vulkanCoreInfo, VkCommandPool commandPool, std::vector<Vertex>& vertices, glm::vec3 chunkLocation);
    void freeWorldVerticesMemory(uint32_t memoryBlockLocation);
    uint32_t addVerticesToUI(VulkanCoreInfo& vulkanCoreInfo, VkCommandPool commandPool, std::vector<Vertex2D>& vertices);
    void freeUIVerticesMemory(uint32_t memoryBlockLocation);

    void getWorldGeometryForRendering(
        VkBuffer& vertexBuffer,
        std::vector<WorldDrawCallData>& vertexOffsets,
        VkBuffer& indexBuffer);

    void getUIGeometryForRendering(
        VkBuffer& vertexBuffer,
        std::vector<VkDeviceSize>& vertexOffsets,
        std::vector<uint32_t>& batchIndexCounts,
        VkBuffer& quadStripIndexBuffer);

    void cleanUp(VulkanCoreInfo& vulkanCoreInfo);

    // public for debugging
    GPUMemoryBlock worldGPUMemoryBlock;
    WorldVertexTracker worldVertexTracker;

    GPUMemoryBlock uiGPUMemoryBlock;

    QuadStripIndexBuffer quadStripIndexBuffer;
};