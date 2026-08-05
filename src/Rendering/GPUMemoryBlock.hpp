#pragma once

#include <cstring>
#include <vector>

#include "VulkanRendering/VulkanTypes.hpp"
#include "util/tlsfAllocator.hpp"

// Buffer uses VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
struct GpuMemoryBlock
{
    VkBuffer buffer;
    VkDeviceMemory deviceMemory;

    bool isHostVisible;                    // Should not be modified.
    void* hostVisibleMappedData = nullptr; // only valid if isHostVisible.

    uint64_t dataSize = 0;
    uint64_t bufferSize = 0;
    uint64_t batchMaxDataCount = 0;

    TlsfAllocator allocator;
};

void gpuMemoryBlockInit(VulkanCoreInfo& vulkanCoreInfo,
                        GpuMemoryBlock& gpuMemoryBlock,
                        uint64_t dataSize,
                        uint64_t bufferSize,
                        uint64_t batchMaxDataCount,
                        bool isHostVisible);
void gpuMemoryBlockGetData(GpuMemoryBlock& gpuMemoryBlock,
                           VkBuffer& outVertexBuffer,
                           std::vector<VkDeviceSize>& vertexOffsets,
                           std::vector<uint64_t>& batchVertexCounts);
void gpuMemoryBlockGetDataMerged(GpuMemoryBlock& gpuMemoryBlock,
                                 VkBuffer& outVertexBuffer,
                                 std::vector<VkDeviceSize>& vertexOffsets,
                                 std::vector<uint32_t>& batchVertexCounts);
void gpuMemoryBlockFree(GpuMemoryBlock& gpuMemoryBlock, uint64_t location);
uint64_t gpuMemoryBlockAddDeviceLocal(VulkanCoreInfo& vulkanCoreInfo,
                                      VkCommandPool commandPool,
                                      GpuMemoryBlock& gpuMemoryBlock,
                                      void* data,
                                      uint64_t size);
uint64_t gpuMemoryBlockAddHostVisible(GpuMemoryBlock& gpuMemoryBlock, void* data, uint64_t size);
void gpuMemoryBlockFreeAll(GpuMemoryBlock& gpuMemoryBlock);
void gpuMemoryBlockDebugPrint(GpuMemoryBlock& gpuMemoryBlock);
uint64_t gpuMemoryBlockDataCount(GpuMemoryBlock& gpuMemoryBlock);
void gpuMemoryBlockDestroy(VulkanCoreInfo& vulkanCoreInfo, GpuMemoryBlock& gpuMemoryBlock);
