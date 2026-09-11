#include "VertexBufferManager.hpp"

#include <iostream>

#include "Constants.hpp"
#include "GPUMemoryBlock.hpp"
#include "vulkan/vulkan_core.h"

VertexBufferManager::VertexBufferManager(VulkanCoreInfo& vulkanCoreInfo,
                                         VkCommandPool commandPool,
                                         uint64_t worldVertexBufferSize)
{
    quadStripIndexBuffer = QuadStripIndexBuffer(vulkanCoreInfo, commandPool, INDEX_BUFFER_QUAD_COUNT);
    worldGpuMemoryBlock = new GpuMemoryBlock;
    gpuMemoryBlockInit(vulkanCoreInfo,
                       *worldGpuMemoryBlock,
                       worldVertexBufferSize,
                       false,
                       VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
                       VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT,
                       VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT);
}

uint64_t VertexBufferManager::addVerticesToWorld(VulkanCoreInfo& vulkanCoreInfo,
                                                 VkCommandPool commandPool,
                                                 std::vector<Vertex>& vertices,
                                                 glm::ivec3 chunkLocation)
{

    size_t dataSize = vertices.size() * sizeof(Vertex);
    uint64_t memoryLocation = gpuMemoryBlockAddDeviceLocal(
        vulkanCoreInfo, commandPool, *worldGpuMemoryBlock, (void*)vertices.data(), dataSize);
    worldVertexTracker.addLocation(static_cast<VkDeviceSize>(memoryLocation), dataSize, chunkLocation, 0, true);
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
