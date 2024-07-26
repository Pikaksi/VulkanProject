#include "VertexBufferManager.hpp"

VertexBufferManager::VertexBufferManager(VulkanCoreInfo& vulkanCoreInfo, VkCommandPool commandPool, uint32_t worldMaxVertexCount, uint32_t uiMaxVertexCount)
{
    uint32_t indexBufferQuadCount = 100000;
    quadStripIndexBuffer = QuadStripIndexBuffer(vulkanCoreInfo, commandPool, indexBufferQuadCount);
    worldGPUMemoryBlock = GPUMemoryBlock(vulkanCoreInfo, sizeof(Vertex), worldMaxVertexCount, quadStripIndexBuffer.getVertexCount());
    uiGPUMemoryBlock = GPUMemoryBlock(vulkanCoreInfo, sizeof(Vertex2D), uiMaxVertexCount, quadStripIndexBuffer.getVertexCount());
}

uint32_t VertexBufferManager::addVerticesToWorld(VulkanCoreInfo& vulkanCoreInfo, VkCommandPool commandPool, std::vector<Vertex>& vertices)
{
    uint32_t memoryLocation;
    worldGPUMemoryBlock.addData<Vertex>(vulkanCoreInfo, commandPool, memoryLocation, vertices);
    return memoryLocation;
}

void VertexBufferManager::freeWorldVerticesMemory(uint32_t memoryBlockLocation)
{
    worldGPUMemoryBlock.freeMemory(memoryBlockLocation);
}


uint32_t VertexBufferManager::addVerticesToUI(VulkanCoreInfo& vulkanCoreInfo, VkCommandPool commandPool, std::vector<Vertex2D>& vertices)
{
    uint32_t memoryLocation;
    uiGPUMemoryBlock.addData<Vertex2D>(vulkanCoreInfo, commandPool, memoryLocation, vertices);
    return memoryLocation;
}

void VertexBufferManager::freeUIVerticesMemory(uint32_t memoryBlockLocation)
{
    uiGPUMemoryBlock.freeMemory(memoryBlockLocation);
}

void VertexBufferManager::getWorldGeometryForRendering(
    VkBuffer& vertexBuffer,
    std::vector<VkDeviceSize>& vertexOffsets,
    std::vector<uint32_t>& batchIndexCounts,
    VkBuffer& indexBuffer)
{
    worldGPUMemoryBlock.getVertexDataMerged(
        vertexBuffer,
        vertexOffsets,
        batchIndexCounts);
    indexBuffer = quadStripIndexBuffer.getBuffer();
}

void VertexBufferManager::getUIGeometryForRendering(
    VkBuffer& vertexBuffer,
    std::vector<VkDeviceSize>& vertexOffsets,
    std::vector<uint32_t>& batchIndexCounts,
    VkBuffer& indexBuffer)
{
    uiGPUMemoryBlock.getVertexDataMerged(
        vertexBuffer,
        vertexOffsets,
        batchIndexCounts);
    indexBuffer = quadStripIndexBuffer.getBuffer();
}

void VertexBufferManager::cleanUp(VulkanCoreInfo& vulkanCoreInfo)
{
    worldGPUMemoryBlock.cleanup(vulkanCoreInfo);
    uiGPUMemoryBlock.cleanup(vulkanCoreInfo);
    quadStripIndexBuffer.cleanUp(vulkanCoreInfo);
}