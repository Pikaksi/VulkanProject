#pragma once

#include <vector>

#include "Vertex.hpp"
#include "VulkanRendering/VulkanTypes.hpp"
#include "VulkanRendering/Buffers.hpp"
#include "Rendering/GPUMemoryBlock.hpp"
#include "QuadStripIndexBuffer.hpp"

class VertexBufferManager
{
public:
    VertexBufferManager() {}
    VertexBufferManager(VulkanCoreInfo* vulkanCoreInfo, VkCommandPool commandPool, VkDeviceSize size);

    uint32_t addChunkVertices(VulkanCoreInfo* vulkanCoreInfo, VkCommandPool commandPool, std::vector<Vertex>& vertices);
    void freeChunkVerticesMemory(uint32_t memoryBlockLocation);
    void getWorldGeometryForRendering(
        VkBuffer& vertexBuffer,
        std::vector<VkDeviceSize>& vertexOffsets,
        std::vector<uint32_t>& batchIndexCounts,
        VkBuffer& quadStripIndexBuffer);

    void cleanUp(VulkanCoreInfo* vulkanCoreInfo);

    GPUMemoryBlock worldGPUMemoryBlock;
private:
    void createGPUMemoryBlocks(VulkanCoreInfo* vulkanCoreInfo, VkCommandPool commandPool, VkDeviceSize size);
    void createQuadStripIndexBuffer(VulkanCoreInfo* vulkanCoreInfo, VkCommandPool commandPool, uint32_t quadCount);
    
    QuadStripIndexBuffer quadStripIndexBuffer;
};