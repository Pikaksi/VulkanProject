#pragma once

#include "VulkanTypes.hpp"

void createGraphicsPipeline3d(VulkanCoreInfo* vulkanCoreInfo, SwapChainInfo* swapChainInfo, GraphicsPipelineInfo* graphicsPipelineInfo, VkDescriptorSetLayout descriptorSetLayout);
void createGraphicsPipeline2d(VulkanCoreInfo* vulkanCoreInfo, SwapChainInfo* swapChainInfo, GraphicsPipelineInfo* graphicsPipelineInfo, VkDescriptorSetLayout descriptorSetLayout);