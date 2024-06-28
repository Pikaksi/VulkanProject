#include "VertexBufferManager.hpp"

VertexBufferManager::VertexBufferManager(VulkanCoreInfo* vulkanCoreInfo, VkCommandPool commandPool, VkDeviceSize size)
{
    createGPUMemoryBlocks(vulkanCoreInfo, commandPool, size);
    quadStripIndexBuffer = QuadStripIndexBuffer(vulkanCoreInfo, commandPool, 100000);
}

uint32_t VertexBufferManager::addChunkVertices(VulkanCoreInfo* vulkanCoreInfo, VkCommandPool commandPool, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices)
{
    uint32_t memoryLocation;
    worldGPUMemoryBlock.addVertices(vulkanCoreInfo, commandPool, memoryLocation, vertices);
    return memoryLocation;
}

void VertexBufferManager::getWorldGeometryForRendering(
    VkBuffer& vertexBuffer,
    std::vector<VkDeviceSize>& vertexOffsets,
    std::vector<uint32_t>& batchIndexCounts,
    VkBuffer& indexBuffer)
{
    worldGPUMemoryBlock.getVerticesToRender(
        vertexBuffer,
        vertexOffsets,
        batchIndexCounts);
    indexBuffer = quadStripIndexBuffer.getBuffer();
}

void VertexBufferManager::freeChunkVerticesMemory(uint32_t memoryBlockLocation)
{
    worldGPUMemoryBlock.freeMemory(memoryBlockLocation);
}

void VertexBufferManager::createGPUMemoryBlocks(VulkanCoreInfo* vulkanCoreInfo, VkCommandPool commandPool, VkDeviceSize size)
{
    worldGPUMemoryBlock = GPUMemoryBlock(vulkanCoreInfo, size, quadStripIndexBuffer.getIndexCount());
}

void VertexBufferManager::cleanUp(VulkanCoreInfo* vulkanCoreInfo)
{
    worldGPUMemoryBlock.cleanup(vulkanCoreInfo);

    quadStripIndexBuffer.cleanUp(vulkanCoreInfo);
}