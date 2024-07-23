#pragma once

#include <vector>

#include "Rendering/Vertex.hpp"
#include "VulkanRendering/VulkanTypes.hpp"
#include "VulkanRendering/Buffers.hpp"
#include "Rendering/GPUMemoryBlock.hpp"
#include "3dRendering/QuadStripIndexBuffer.hpp"

class VertexBufferManager
{
public:
    VertexBufferManager() {}
    VertexBufferManager(VulkanCoreInfo* vulkanCoreInfo, VkCommandPool commandPool, uint32_t worldVertexCount, uint32_t uiMaxVertexCount);

    uint32_t addVerticesToWorld(VulkanCoreInfo* vulkanCoreInfo, VkCommandPool commandPool, std::vector<Vertex>& vertices);
    void freeWorldVerticesMemory(uint32_t memoryBlockLocation);
    uint32_t addVerticesToUI(VulkanCoreInfo* vulkanCoreInfo, VkCommandPool commandPool, std::vector<Vertex2D>& vertices);
    void freeUIVerticesMemory(uint32_t memoryBlockLocation);
    void getWorldGeometryForRendering(
        VkBuffer& vertexBuffer,
        std::vector<VkDeviceSize>& vertexOffsets,
        std::vector<uint32_t>& batchIndexCounts,
        VkBuffer& quadStripIndexBuffer);
    void getUIGeometryForRendering(
        VkBuffer& vertexBuffer,
        std::vector<VkDeviceSize>& vertexOffsets,
        std::vector<uint32_t>& batchIndexCounts,
        VkBuffer& quadStripIndexBuffer);

    void cleanUp(VulkanCoreInfo* vulkanCoreInfo);

    // public for debugging
    GPUMemoryBlock worldGPUMemoryBlock;
    GPUMemoryBlock uiGPUMemoryBlock;
    QuadStripIndexBuffer quadStripIndexBuffer;
};