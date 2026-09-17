#include "indirectDrawCallRecorder.hpp"
#include "Buffers.hpp"
#include "VulkanTypes.hpp"
#include "vulkan/vulkan_core.h"

#include <cstring>

uint64_t getRequiredRecorderSize(DrawCallRecorder& recorder)
{
    return recorder.capacity * (recorder.drawCommandSize + recorder.pushConstantSize);
}

void drawCallRecorderCreateBuffers(DrawCallRecorder& recorder, VulkanCoreInfo& vulkanCoreInfo)
{
    createBuffer(vulkanCoreInfo,
                 recorder.size,
                 VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 0,
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
    uint64_t oldPushOffset = recorder.drawCommandSize * recorder.capacity;
    uint64_t pushSize = recorder.pushConstantSize * recorder.size;

    recorder.capacity *= 2;
    drawCallRecorderCreateBuffers(recorder, vulkanCoreInfo);

    uint64_t newPushOffset = recorder.drawCommandSize * recorder.capacity;

    memcpy(recorder.mappedData, mappedData, drawSize);
    memcpy((void*)((char*)recorder.mappedData + newPushOffset), (void*)((char*)mappedData + oldPushOffset), pushSize);

    vkUnmapMemory(vulkanCoreInfo.device, deviceMemory);
    vkDestroyBuffer(vulkanCoreInfo.device, buffer, nullptr);
    vkFreeMemory(vulkanCoreInfo.device, deviceMemory, nullptr);
}

void drawCallRecorderAdd(DrawCallRecorder& recorder,
                         VulkanCoreInfo& vulkanCoreInfo,
                         VkDrawIndirectCommand& command,
                         void* pushConstant)
{
    if (recorder.capacity <= recorder.size) {
        drawCallRecorderIncreaseCapacity(recorder, vulkanCoreInfo);
    }

    uint64_t drawCommandOffset = recorder.size * recorder.drawCommandSize;
    memcpy((void*)((char*)recorder.mappedData + drawCommandOffset), (void*)&command, recorder.drawCommandSize);

    uint64_t pushConstantOffset = recorder.capacity * recorder.drawCommandSize + recorder.size * recorder.pushConstantSize;
    memcpy((void*)((char*)recorder.mappedData + pushConstantOffset), pushConstant, recorder.pushConstantSize);
}

void drawCallRecorderGetRenderingParameters(DrawCallRecorder& recorder,
                                            uint64_t& drawsOffset,
                                            uint64_t& draws,
                                            uint64_t& pushConstantsOffset,
                                            uint64_t& pushConstants)
{
    drawsOffset = 0;
    draws = recorder.size;
    pushConstantsOffset = recorder.size * recorder.drawCommandSize;
}

/*void createDrawCallBuffers(VulkanCoreInfo& vulkanCoreInfo, std::vector<GpuMemoryBlock>& drawCallBuffers)
{
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        drawCallBuffers.push_back(GpuMemoryBlock{});
        gpuMemoryBlockInit(vulkanCoreInfo,
                           drawCallBuffers.back(),
                           maxDrawCalls * sizeof(VkDrawIndirectCommand) +
                               maxDrawCalls * std::max(sizeof(PushConstant3d), sizeof(PushConstant3dLod)),
                           true,
                           VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                           0);
    }
}*/
