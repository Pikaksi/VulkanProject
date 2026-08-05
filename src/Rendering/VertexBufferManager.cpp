#include "VertexBufferManager.hpp"
#include "Constants.hpp"
#include "GPUMemoryBlock.hpp"

VertexBufferManager::VertexBufferManager(VulkanCoreInfo& vulkanCoreInfo,
                                         VkCommandPool commandPool,
                                         uint32_t worldMaxVertexCount,
                                         uint32_t uiMaxVertexCount)
{
    quadStripIndexBuffer = QuadStripIndexBuffer(vulkanCoreInfo, commandPool, INDEX_BUFFER_QUAD_COUNT);
    worldGpuMemoryBlock = new GpuMemoryBlock;
    gpuMemoryBlockInit(vulkanCoreInfo,
                       *worldGpuMemoryBlock,
                       sizeof(Vertex),
                       sizeof(Vertex) * worldMaxVertexCount,
                       quadStripIndexBuffer.getVertexCount(),
                       false);
}

uint32_t VertexBufferManager::addVerticesToWorld(VulkanCoreInfo& vulkanCoreInfo,
                                                 VkCommandPool commandPool,
                                                 std::vector<Vertex>& vertices,
                                                 glm::ivec3 chunkLocation)
{
    uint32_t memoryLocation = gpuMemoryBlockAddDeviceLocal(
        vulkanCoreInfo, commandPool, *worldGpuMemoryBlock, (void*)vertices.data(), sizeof(Vertex) * vertices.size());
    worldVertexTracker.addLocation(static_cast<VkDeviceSize>(memoryLocation), vertices.size(), chunkLocation);
    return memoryLocation;
}

void VertexBufferManager::freeWorldVerticesMemory(uint32_t memoryBlockLocation)
{
    gpuMemoryBlockFree(*worldGpuMemoryBlock, memoryBlockLocation);
    worldVertexTracker.removeLocation(static_cast<VkDeviceSize>(memoryBlockLocation));
}

void VertexBufferManager::getWorldGeometryForRendering(VkBuffer& vertexBuffer,
                                                       std::vector<WorldDrawCallData>& vertexOffsets,
                                                       VkBuffer& indexBuffer)
{
    vertexBuffer = worldGpuMemoryBlock->buffer;
    vertexOffsets = worldVertexTracker.getData();
    indexBuffer = quadStripIndexBuffer.getBuffer();
}

void VertexBufferManager::cleanUp(VulkanCoreInfo& vulkanCoreInfo)
{
    gpuMemoryBlockDestroy(vulkanCoreInfo, *worldGpuMemoryBlock);
    quadStripIndexBuffer.cleanUp(vulkanCoreInfo);
}
