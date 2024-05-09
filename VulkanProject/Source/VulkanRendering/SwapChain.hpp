#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>

#include "VulkanTypes.hpp"

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

SwapChainSupportDetails querySwapChainSupport(VulkanCoreInfo* vulkanCoreInfo);
VkFormat findDepthFormat(VulkanCoreInfo* vulkanCoreInfo);

void createSwapChain(VulkanCoreInfo* vulkanCoreInfo, SwapChainInfo* swapChainInfo);
void cleanupSwapChain(VulkanCoreInfo* vulkanCoreInfo, SwapChainInfo* swapChainInfo);
void recreateSwapChain(VulkanCoreInfo* vulkanCoreInfo, SwapChainInfo* swapChainInfo);