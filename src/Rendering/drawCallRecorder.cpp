#include "drawCallRecorder.hpp"
#include "Buffers.hpp"
#include "VulkanTypes.hpp"
#include "vulkan/vulkan_core.h"
#include "assertm.hpp"

#include <cstring>

uint64_t getRequiredRecorderSize(DrawCallRecorder& recorder)
{
    // 16 bytes extra so that the push constants cal always be padded to 16 byte alignment
    return recorder.capacity * (recorder.drawCommandSize + recorder.pushConstantSize) + 16;
}

void drawCallRecorderCreateBuffers(DrawCallRecorder& recorder, VulkanCoreInfo& vulkanCoreInfo)
{
    recorder.pushConstantsOffset = (recorder.capacity * recorder.drawCommandSize + 15) / 16 * 16;

    createBuffer(vulkanCoreInfo,
                 getRequiredRecorderSize(recorder),
                 VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT,
                 recorder.buffer,
                 recorder.deviceMemory);

    vkMapMemory(
        vulkanCoreInfo.device, recorder.deviceMemory, 0, getRequiredRecorderSize(recorder), 0, &recorder.mappedData);
}

void drawCallRecorderInit(DrawCallRecorder& recorder,
                          VulkanCoreInfo& vulkanCoreInfo,
                          uint32_t pushConstantSize,
                          uint64_t startingDrawCalls)
{
    recorder.pushConstantSize = pushConstantSize;
    recorder.capacity = startingDrawCalls;
    recorder.size = 0;

    drawCallRecorderCreateBuffers(recorder, vulkanCoreInfo);
}

void drawCallRecorderDestroy(DrawCallRecorder& recorder, VulkanCoreInfo& vulkanCoreInfo)
{
    vkUnmapMemory(vulkanCoreInfo.device, recorder.deviceMemory);
    vkDestroyBuffer(vulkanCoreInfo.device, recorder.buffer, nullptr);
    vkFreeMemory(vulkanCoreInfo.device, recorder.deviceMemory, nullptr);
}

void drawCallRecorderIncreaseCapacity(DrawCallRecorder& recorder, VulkanCoreInfo& vulkanCoreInfo)
{
    void* mappedData = recorder.mappedData;
    VkBuffer buffer = recorder.buffer;
    VkDeviceMemory deviceMemory = recorder.deviceMemory;

    uint64_t drawSize = recorder.drawCommandSize * recorder.size;
    uint64_t oldPushOffset = recorder.pushConstantsOffset;
    uint64_t pushSize = recorder.pushConstantSize * recorder.size;

    recorder.capacity *= 2;
    drawCallRecorderCreateBuffers(recorder, vulkanCoreInfo);

    uint64_t newPushOffset = recorder.pushConstantsOffset;

    memcpy(recorder.mappedData, mappedData, drawSize);
    memcpy((void*)((char*)recorder.mappedData + newPushOffset), (void*)((char*)mappedData + oldPushOffset), pushSize);

    vkUnmapMemory(vulkanCoreInfo.device, deviceMemory);
    vkDestroyBuffer(vulkanCoreInfo.device, buffer, nullptr);
    vkFreeMemory(vulkanCoreInfo.device, deviceMemory, nullptr);
}

void drawCallRecorderAdd(DrawCallRecorder& recorder,
                         VulkanCoreInfo& vulkanCoreInfo,
                         VkDrawIndexedIndirectCommand& command,
                         void* pushConstant)
{
    if (recorder.capacity <= recorder.size) {
        drawCallRecorderIncreaseCapacity(recorder, vulkanCoreInfo);
    }

    uint64_t drawCommandLocation = recorder.size * recorder.drawCommandSize;
    uint64_t pushConstantLocation = recorder.pushConstantsOffset + recorder.size * recorder.pushConstantSize;

    assertm(getRequiredRecorderSize(recorder) > pushConstantLocation, "Recorder is not big enough");
    assertm(getRequiredRecorderSize(recorder) > drawCommandLocation, "Recorder is not big enough");

    memcpy((void*)((char*)recorder.mappedData + drawCommandLocation), (void*)&command, recorder.drawCommandSize);
    memcpy((void*)((char*)recorder.mappedData + pushConstantLocation), pushConstant, recorder.pushConstantSize);

    recorder.size += 1;
}

DrawCallRecorderDrawParameters drawCallRecorderGetRenderingParameters(DrawCallRecorder& recorder,
                                                                      VulkanCoreInfo& vulkanCoreInfo)
{
    assertm(recorder.buffer != VK_NULL_HANDLE, "Buffer is VK_NULL_HANDLE, Capacity is " << recorder.capacity);
    assertm(recorder.pushConstantsOffset / 16 * 16 == recorder.pushConstantsOffset, "Push constants are not aligned to 16 bytes");

    VkBufferDeviceAddressInfo addressInfo{};
    addressInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
    addressInfo.buffer = recorder.buffer;
    VkDeviceAddress pushConstantsPointer = vkGetBufferDeviceAddress(vulkanCoreInfo.device, &addressInfo);
    pushConstantsPointer += recorder.pushConstantsOffset;

    return DrawCallRecorderDrawParameters{
        .drawsBuffer = recorder.buffer,
        .pushConstantsDevicePointer = pushConstantsPointer,
        .draws = recorder.size,
    };
}

void drawCallRecorderReset(DrawCallRecorder& recorder)
{
    recorder.size = 0;
}
