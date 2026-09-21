#pragma once

#include "VulkanTypes.hpp"
#include "Constants.hpp"
#include "vulkan/vulkan_core.h"

struct DrawCallRecorder
{
    static const uint32_t drawCommandSize = sizeof(VkDrawIndexedIndirectCommand);
    uint32_t pushConstantSize = 0;

    uint64_t size = 0;
    uint64_t capacity = 0;
    uint64_t pushConstantsOffset = 0;

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
                         VkDrawIndexedIndirectCommand& command,
                         void* pushConstant);

struct DrawCallRecorderDrawParameters {
    uint64_t draws;
    VkBuffer drawsBuffer;
    VkDeviceAddress pushConstantsDevicePointer;
};

DrawCallRecorderDrawParameters drawCallRecorderGetRenderingParameters(DrawCallRecorder& recorder,
                                                                      VulkanCoreInfo& vulkanCoreInfo);

void drawCallRecorderReset(DrawCallRecorder& recorder);
