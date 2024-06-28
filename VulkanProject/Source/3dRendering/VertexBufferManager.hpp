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

    uint32_t addChunkVertices(VulkanCoreInfo* vulkanCoreInfo, VkCommandPool commandPool, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);
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
    
    QuadStripIndexBuffer quadStripIndexBuffer;
};