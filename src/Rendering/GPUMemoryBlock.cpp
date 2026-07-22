#include "GPUMemoryBlock.hpp"
#include "tlsfAllocator.hpp"
#include "util/assertm.hpp"

#include <iostream>

void gpuMemoryBlockInit(VulkanCoreInfo& vulkanCoreInfo,
                        GpuMemoryBlock& gpuMemoryBlock,
                        uint64_t dataSize_,
                        uint64_t bufferSize_,
                        uint64_t batchMaxDataCount_)
{
    gpuMemoryBlock.dataSize = dataSize_;
    gpuMemoryBlock.bufferSize = bufferSize_;
    gpuMemoryBlock.batchMaxDataCount = batchMaxDataCount_;

    tlsfInit(gpuMemoryBlock.allocator, gpuMemoryBlock.bufferSize);

    createBuffer(vulkanCoreInfo,
                 gpuMemoryBlock.bufferSize,
                 VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                 gpuMemoryBlock.buffer,
                 gpuMemoryBlock.deviceMemory);
}

void gpuMemoryBlockGetData(VkBuffer& outVertexBuffer,
                           GpuMemoryBlock& gpuMemoryBlock,
                           std::vector<VkDeviceSize>& vertexOffsets,
                           std::vector<uint64_t>& batchVertexCounts)
{
    outVertexBuffer = gpuMemoryBlock.buffer;

    TlsfBlockHeader* block = gpuMemoryBlock.allocator.firstPhysicalBlock;

    while (block != nullptr) {
        if (block->free == false) {
            vertexOffsets.push_back(block->location);
            batchVertexCounts.push_back(block->size / gpuMemoryBlock.dataSize);
        }
        block = block->nextPhysical;
    }
}

void gpuMemoryBlockGetDataMerged(VkBuffer& outVertexBuffer,
                                 GpuMemoryBlock& gpuMemoryBlock,
                                 std::vector<VkDeviceSize>& vertexOffsets,
                                 std::vector<uint32_t>& batchVertexCounts)
{
    outVertexBuffer = gpuMemoryBlock.buffer;

    TlsfBlockHeader* block = gpuMemoryBlock.allocator.firstPhysicalBlock;
    bool lastBlockFree = true;

    while (block != nullptr) {
        if (block->free == false) {
            if (lastBlockFree == false && batchVertexCounts.back() + block->size <= gpuMemoryBlock.batchMaxDataCount) {
                batchVertexCounts.back() += block->size;
            }
            else {
                vertexOffsets.push_back(block->location);
                batchVertexCounts.push_back(block->size / gpuMemoryBlock.dataSize);
            }
        }
        lastBlockFree = block->free;
        block = block->nextPhysical;
    }
}

void gpuMemoryBlockCopyData(VulkanCoreInfo& vulkanCoreInfo,
                            VkCommandPool commandPool,
                            GpuMemoryBlock& gpuMemoryBlock,
                            uint64_t location,
                            uint64_t size,
                            void* data)
{
    assertm(location + size < gpuMemoryBlock.bufferSize, "Tried to copy memory out of buffer");

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(vulkanCoreInfo,
                 size,
                 VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 stagingBuffer,
                 stagingBufferMemory);

    void* stagingBufferPointer;
    vkMapMemory(vulkanCoreInfo.device, stagingBufferMemory, 0, size, 0, &stagingBufferPointer);
    memcpy(stagingBufferPointer, data, (size_t)size);
    vkUnmapMemory(vulkanCoreInfo.device, stagingBufferMemory);

    copyBuffer(vulkanCoreInfo, commandPool, stagingBuffer, gpuMemoryBlock.buffer, size, 0, location);

    vkDestroyBuffer(vulkanCoreInfo.device, stagingBuffer, nullptr);
    vkFreeMemory(vulkanCoreInfo.device, stagingBufferMemory, nullptr);
}

void gpuMemoryBlockFree(GpuMemoryBlock& gpuMemoryBlock, uint64_t location)
{
    tlsfFree(gpuMemoryBlock.allocator, location);
}

uint64_t gpuMemoryBlockAdd(VulkanCoreInfo& vulkanCoreInfo,
                           VkCommandPool commandPool,
                           GpuMemoryBlock& gpuMemoryBlock,
                           void* data,
                           uint64_t size)
{
    uint64_t memoryLocation = tlsfAllocate(gpuMemoryBlock.allocator, size);

    gpuMemoryBlockCopyData(vulkanCoreInfo, commandPool, gpuMemoryBlock, memoryLocation, size, data);

    return memoryLocation;
}

void gpuMemoryBlockDebugPrint(GpuMemoryBlock& gpuMemoryBlock) { tlsfDebugPrint(gpuMemoryBlock.allocator); }

uint64_t gpuMemoryBlockDataCount(GpuMemoryBlock& gpuMemoryBlock)
{
    uint64_t dataSize = 0;

    TlsfBlockHeader* block = gpuMemoryBlock.allocator.firstPhysicalBlock;

    while (block != nullptr) {
        if (block->free == false) {
            dataSize += block->size;
        }
        block = block->nextPhysical;
    }
    return dataSize / gpuMemoryBlock.dataSize;
}

void gpuMemoryBlockDestroy(VulkanCoreInfo& vulkanCoreInfo, GpuMemoryBlock& gpuMemoryBlock)
{
    vkDestroyBuffer(vulkanCoreInfo.device, gpuMemoryBlock.buffer, nullptr);
    vkFreeMemory(vulkanCoreInfo.device, gpuMemoryBlock.deviceMemory, nullptr);
    tlsfDestroy(gpuMemoryBlock.allocator);
}
