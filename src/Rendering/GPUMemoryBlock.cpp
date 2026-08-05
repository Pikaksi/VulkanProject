#include "GPUMemoryBlock.hpp"
#include "tlsfAllocator.hpp"
#include "util/assertm.hpp"
#include "vulkan/vulkan_core.h"
#include "Buffers.hpp"

void gpuMemoryBlockInit(VulkanCoreInfo& vulkanCoreInfo,
                        GpuMemoryBlock& gpuMemoryBlock,
                        uint64_t dataSize,
                        uint64_t bufferSize,
                        uint64_t batchMaxDataCount,
                        bool isHostVisible)
{
    gpuMemoryBlock.dataSize = dataSize;
    gpuMemoryBlock.bufferSize = bufferSize;
    gpuMemoryBlock.batchMaxDataCount = batchMaxDataCount;
    gpuMemoryBlock.isHostVisible = isHostVisible;

    tlsfInit(gpuMemoryBlock.allocator, gpuMemoryBlock.bufferSize);

    if (gpuMemoryBlock.isHostVisible) {
        createBuffer(vulkanCoreInfo,
                     gpuMemoryBlock.bufferSize,
                     VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, // TODO: use device local and host coherent? maybe perf gain.
                     gpuMemoryBlock.buffer,
                     gpuMemoryBlock.deviceMemory);

        vkMapMemory(vulkanCoreInfo.device,
                    gpuMemoryBlock.deviceMemory,
                    0,
                    gpuMemoryBlock.bufferSize,
                    0,
                    &gpuMemoryBlock.hostVisibleMappedData);
    }
    else {
        createBuffer(vulkanCoreInfo,
                     gpuMemoryBlock.bufferSize,
                     VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                     gpuMemoryBlock.buffer,
                     gpuMemoryBlock.deviceMemory);
    }
}

void gpuMemoryBlockGetData(GpuMemoryBlock& gpuMemoryBlock,
                           VkBuffer& outVertexBuffer,
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

void gpuMemoryBlockGetDataMerged(GpuMemoryBlock& gpuMemoryBlock,
                                 VkBuffer& outVertexBuffer,
                                 std::vector<VkDeviceSize>& vertexOffsets,
                                 std::vector<uint32_t>& batchVertexCounts)
{
    outVertexBuffer = gpuMemoryBlock.buffer;

    TlsfBlockHeader* block = gpuMemoryBlock.allocator.firstPhysicalBlock;
    bool lastBlockFree = true;

    while (block != nullptr) {
        if (block->free == false) {
            if (lastBlockFree == false &&
                batchVertexCounts.back() + block->size <= gpuMemoryBlock.batchMaxDataCount * gpuMemoryBlock.dataSize) {
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

void gpuMemoryBlockFreeAll(GpuMemoryBlock& gpuMemoryBlock)
{
    tlsfFreeAll(gpuMemoryBlock.allocator);
}

void gpuMemoryBlockCopyDataHostVisible(GpuMemoryBlock& gpuMemoryBlock, uint64_t location, uint64_t size, void* data)
{
    assertm(location + size < gpuMemoryBlock.bufferSize, "Tried to copy memory out of buffer");

    memcpy(gpuMemoryBlock.hostVisibleMappedData, data, (size_t)size);
}

void gpuMemoryBlockFree(GpuMemoryBlock& gpuMemoryBlock, uint64_t location)
{
    tlsfFree(gpuMemoryBlock.allocator, location);
}

uint64_t gpuMemoryBlockAddHostVisible(GpuMemoryBlock& gpuMemoryBlock, void* data, uint64_t size)
{
    assertm(gpuMemoryBlock.isHostVisible == true,
            "gpuMemoryBlock was not host visible. Instead of gpuMemoryBlockAddHostVisible use "
            "gpuMemoryBlockAddDeviceLocal");

    uint64_t memoryLocation = tlsfAllocate(gpuMemoryBlock.allocator, size);

    assertm(memoryLocation + size < gpuMemoryBlock.bufferSize, "Tried to copy memory out of buffer");

    memcpy(gpuMemoryBlock.hostVisibleMappedData, data, (size_t)size);

    return memoryLocation;
}

uint64_t gpuMemoryBlockAddDeviceLocal(VulkanCoreInfo& vulkanCoreInfo,
                                      VkCommandPool commandPool,
                                      GpuMemoryBlock& gpuMemoryBlock,
                                      void* data,
                                      uint64_t size)
{
    assertm(gpuMemoryBlock.isHostVisible == false,
            "gpuMemoryBlock was not device local. Instead of gpuMemoryBlockAddDeviceLocal use "
            "gpuMemoryBlockAddHostVisible");

    uint64_t memoryLocation = tlsfAllocate(gpuMemoryBlock.allocator, size);

    assertm(memoryLocation + size < gpuMemoryBlock.bufferSize, "Tried to copy memory out of buffer");

    // auto debugStartWait = std::chrono::high_resolution_clock::now();

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

    copyBuffer(vulkanCoreInfo, commandPool, stagingBuffer, gpuMemoryBlock.buffer, size, 0, memoryLocation);

    vkDestroyBuffer(vulkanCoreInfo.device, stagingBuffer, nullptr);
    vkFreeMemory(vulkanCoreInfo.device, stagingBufferMemory, nullptr);

    /*auto debugEndWait = std::chrono::high_resolution_clock::now();
    auto uploadTime =
        std::chrono::duration<float, std::chrono::milliseconds::period>(debugEndWait - debugStartWait).count();
    std::cout << "time is " << uploadTime << std::endl;*/

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
    if (gpuMemoryBlock.isHostVisible) {
        vkUnmapMemory(vulkanCoreInfo.device, gpuMemoryBlock.deviceMemory);
    }
    vkDestroyBuffer(vulkanCoreInfo.device, gpuMemoryBlock.buffer, nullptr);
    vkFreeMemory(vulkanCoreInfo.device, gpuMemoryBlock.deviceMemory, nullptr);
    tlsfDestroy(gpuMemoryBlock.allocator);
}
