#pragma once

#include "vulkan/vulkan.h"

#include <cstring>
#include <vector>

#include "VulkanRendering/VulkanTypes.hpp"
#include "VulkanRendering/Buffers.hpp"
#include "util/tlsfAllocator.hpp"

// Buffer uses VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
struct GpuMemoryBlock
{
    VkBuffer buffer;
    VkDeviceMemory deviceMemory;

    uint64_t dataSize = 0;
    uint64_t bufferSize = 0;
    uint64_t batchMaxDataCount = 0;

    TlsfAllocator allocator;
};

void gpuMemoryBlockInit(VulkanCoreInfo& vulkanCoreInfo,
                        GpuMemoryBlock& gpuMemoryBlock,
                        uint64_t dataSize,
                        uint64_t bufferSize,
                        uint64_t batchMaxDataCount);
void gpuMemoryBlockGetData(VkBuffer& outVertexBuffer,
                           GpuMemoryBlock& gpuMemoryBlock,
                           std::vector<VkDeviceSize>& vertexOffsets,
                           std::vector<uint64_t>& batchVertexCounts);
void gpuMemoryBlockGetDataMerged(VkBuffer& outVertexBuffer,
                                 GpuMemoryBlock& gpuMemoryBlock,
                                 std::vector<VkDeviceSize>& vertexOffsets,
                                 std::vector<uint32_t>& batchVertexCounts);
void gpuMemoryBlockFree(GpuMemoryBlock& gpuMemoryBlock, uint64_t location);
uint64_t gpuMemoryBlockAdd(VulkanCoreInfo& vulkanCoreInfo,
                               VkCommandPool commandPool,
                               GpuMemoryBlock& gpuMemoryBlock,
                               void* data,
                               uint64_t size);
void gpuMemoryBlockDebugPrint(GpuMemoryBlock& gpuMemoryBlock);
uint64_t gpuMemoryBlockDataCount(GpuMemoryBlock& gpuMemoryBlock);
void gpuMemoryBlockDestroy(VulkanCoreInfo& vulkanCoreInfo, GpuMemoryBlock& gpuMemoryBlock);
