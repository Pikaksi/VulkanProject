#include "VertexBufferManager.hpp"

#include <iostream>

#include "Constants.hpp"
#include "GPUMemoryBlock.hpp"

VertexBufferManager::VertexBufferManager(VulkanCoreInfo& vulkanCoreInfo,
                                         VkCommandPool commandPool,
                                         uint64_t worldVertexBufferSize)
{
    quadStripIndexBuffer = QuadStripIndexBuffer(vulkanCoreInfo, commandPool, INDEX_BUFFER_QUAD_COUNT);
    worldGpuMemoryBlock = new GpuMemoryBlock;
    gpuMemoryBlockInit(vulkanCoreInfo, *worldGpuMemoryBlock, worldVertexBufferSize, false);
}

uint64_t VertexBufferManager::addVerticesToWorld(VulkanCoreInfo& vulkanCoreInfo,
                                                 VkCommandPool commandPool,
                                                 std::vector<Vertex>& vertices,
                                                 glm::ivec3 chunkLocation)
{

    size_t dataSize = vertices.size() * sizeof(Vertex);
    uint64_t memoryLocation = gpuMemoryBlockAddDeviceLocal(
        vulkanCoreInfo, commandPool, *worldGpuMemoryBlock, (void*)vertices.data(), dataSize);
    worldVertexTracker.addLocation(
        static_cast<VkDeviceSize>(memoryLocation), dataSize, chunkLocation, 0, true);
    return memoryLocation;
}

uint64_t VertexBufferManager::addVerticesToWorldLod(VulkanCoreInfo& vulkanCoreInfo,
                                                    VkCommandPool commandPool,
                                                    std::vector<VertexLod>& vertices,
                                                    glm::ivec3 chunkLocation,
                                                    int lod)
{
    uint64_t memoryLocation = gpuMemoryBlockAddDeviceLocal(
        vulkanCoreInfo, commandPool, *worldGpuMemoryBlock, (void*)vertices.data(), sizeof(VertexLod) * vertices.size());
    worldVertexTracker.addLocation(
        static_cast<VkDeviceSize>(memoryLocation), vertices.size() * sizeof(VertexLod), chunkLocation, lod, false);
    return memoryLocation;
}

void VertexBufferManager::freeWorldVerticesMemory(uint64_t memoryBlockLocation)
{
    gpuMemoryBlockFree(*worldGpuMemoryBlock, memoryBlockLocation);
    worldVertexTracker.removeLocation(static_cast<VkDeviceSize>(memoryBlockLocation));
}

void VertexBufferManager::getWorldGeometryForRendering(VkBuffer& vertexBuffer,
                                                       std::vector<WorldDrawCallData>& vertexOffsets,
                                                       VkBuffer& indexBuffer)
{
    vertexBuffer = worldGpuMemoryBlock->buffer;
    vertexOffsets = worldVertexTracker.trackedDrawCallData;
    indexBuffer = quadStripIndexBuffer.getBuffer();
}

void VertexBufferManager::cleanUp(VulkanCoreInfo& vulkanCoreInfo)
{
    gpuMemoryBlockDestroy(vulkanCoreInfo, *worldGpuMemoryBlock);
    quadStripIndexBuffer.cleanUp(vulkanCoreInfo);
}
