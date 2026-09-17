#pragma once

#include "VulkanTypes.hpp"
#include "Constants.hpp"
#include "vulkan/vulkan_core.h"

struct DrawCallRecorder
{
    static const uint32_t drawCommandSize = sizeof(VkDrawIndirectCommand);
    uint32_t pushConstantSize = 0;

    uint64_t size = 0;
    uint64_t capacity = 0;

    void* mappedData;
    VkBuffer buffer;
    VkDeviceMemory deviceMemory;
};

void drawCallRecorderInit(DrawCallRecorder& recorder,
                          VulkanCoreInfo& vulkanCoreInfo,
                          uint32_t pushConstantSize,
                          uint64_t startingDrawCalls);

void drawCallRecorderDestroy(DrawCallRecorder& recorder, VulkanCoreInfo& vulkanCoreInfo);

void drawCallRecorderAdd(DrawCallRecorder& recorder,
                         VulkanCoreInfo& vulkanCoreInfo,
                         VkDrawIndirectCommand command,
                         void* pushConstant);

void drawCallRecorderGetRenderingParameters(DrawCallRecorder& recorder,
                                            uint64_t& drawsOffset,
                                            uint64_t& drawsSize,
                                            uint64_t& pushConstantsOffset,
                                            uint64_t& pushConstantsSize);
