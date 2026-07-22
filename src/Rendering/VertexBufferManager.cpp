#include "VertexBufferManager.hpp"
#include "GPUMemoryBlock.hpp"

VertexBufferManager::VertexBufferManager(VulkanCoreInfo& vulkanCoreInfo,
                                         VkCommandPool commandPool,
                                         uint32_t worldMaxVertexCount,
                                         uint32_t uiMaxVertexCount)
{
    uint32_t indexBufferQuadCount = 100000;
    quadStripIndexBuffer = QuadStripIndexBuffer(vulkanCoreInfo, commandPool, indexBufferQuadCount);
    worldGpuMemoryBlock = new GpuMemoryBlock;
    gpuMemoryBlockInit(vulkanCoreInfo,
                       *worldGpuMemoryBlock,
                       sizeof(Vertex),
                       sizeof(Vertex) * worldMaxVertexCount,
                       quadStripIndexBuffer.getVertexCount());

    uiGpuMemoryBlock = new GpuMemoryBlock;
    gpuMemoryBlockInit(vulkanCoreInfo,
                       *uiGpuMemoryBlock,
                       sizeof(Vertex2D),
                       sizeof(Vertex2D) * uiMaxVertexCount,
                       quadStripIndexBuffer.getVertexCount());
}

uint32_t VertexBufferManager::addVerticesToWorld(VulkanCoreInfo& vulkanCoreInfo,
                                                 VkCommandPool commandPool,
                                                 std::vector<Vertex>& vertices,
                                                 glm::ivec3 chunkLocation)
{
    uint32_t memoryLocation = gpuMemoryBlockAdd(
        vulkanCoreInfo, commandPool, *worldGpuMemoryBlock, (void*)vertices.data(), sizeof(Vertex) * vertices.size());
    worldVertexTracker.addLocation(static_cast<VkDeviceSize>(memoryLocation), vertices.size(), chunkLocation);
    return memoryLocation;
}

void VertexBufferManager::freeWorldVerticesMemory(uint32_t memoryBlockLocation)
{
    gpuMemoryBlockFree(*worldGpuMemoryBlock, memoryBlockLocation);
    worldVertexTracker.removeLocation(static_cast<VkDeviceSize>(memoryBlockLocation));
}

uint64_t VertexBufferManager::addVerticesToUI(VulkanCoreInfo& vulkanCoreInfo,
                                              VkCommandPool commandPool,
                                              std::vector<Vertex2D>& vertices)
{
    uint64_t memoryLocation = gpuMemoryBlockAdd(
        vulkanCoreInfo, commandPool, *uiGpuMemoryBlock, (void*)vertices.data(), sizeof(Vertex2D) * vertices.size());
    return memoryLocation;
}

void VertexBufferManager::freeUIVerticesMemory(uint64_t memoryLocation)
{
    gpuMemoryBlockFree(*uiGpuMemoryBlock, memoryLocation);
}

void VertexBufferManager::getWorldGeometryForRendering(VkBuffer& vertexBuffer,
                                                       std::vector<WorldDrawCallData>& vertexOffsets,
                                                       VkBuffer& indexBuffer)
{
    vertexBuffer = worldGpuMemoryBlock->buffer;
    vertexOffsets = worldVertexTracker.getData();
    indexBuffer = quadStripIndexBuffer.getBuffer();
}

void VertexBufferManager::getUIGeometryForRendering(VkBuffer& vertexBuffer,
                                                    std::vector<VkDeviceSize>& vertexOffsets,
                                                    std::vector<uint32_t>& batchIndexCounts,
                                                    VkBuffer& indexBuffer)
{
    gpuMemoryBlockGetDataMerged(vertexBuffer, *uiGpuMemoryBlock, vertexOffsets, batchIndexCounts);
    indexBuffer = quadStripIndexBuffer.getBuffer();
}

void VertexBufferManager::cleanUp(VulkanCoreInfo& vulkanCoreInfo)
{
    gpuMemoryBlockDestroy(vulkanCoreInfo, *worldGpuMemoryBlock);
    gpuMemoryBlockDestroy(vulkanCoreInfo, *uiGpuMemoryBlock);
    quadStripIndexBuffer.cleanUp(vulkanCoreInfo);
}
