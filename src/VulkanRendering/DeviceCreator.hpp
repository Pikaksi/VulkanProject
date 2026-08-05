#pragma once

#include "VulkanRendering/VulkanTypes.hpp"
#include <optional>

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

QueueFamilyIndices findQueueFamilies(VulkanCoreInfo& vulkanCoreInfo);
void createDevice(VulkanCoreInfo& vulkanCoreInfo);
