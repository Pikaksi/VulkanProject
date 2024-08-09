#pragma once

#include "VulkanTypes.hpp"

VkDescriptorSetLayout createDescriptorSetLayout3d(VulkanCoreInfo& vulkanCoreInfo);
VkDescriptorSetLayout createDescriptorSetLayout2d(VulkanCoreInfo& vulkanCoreInfo);

VkDescriptorPool createDescriptorPool3d(VulkanCoreInfo& vulkanCoreInfo);
VkDescriptorPool createDescriptorPool2d(VulkanCoreInfo& vulkanCoreInfo, uint32_t imageCount);

std::vector<VkDescriptorSet> createDescriptorSets3d(
    VulkanCoreInfo& vulkanCoreInfo,
    VkDescriptorPool descriptorPool,
    VkDescriptorSetLayout descriptorSetLayout,
    std::vector<UniformBufferInfo>& cameraUniformBuffers,
    ImageInfo& textureImage,
    VkSampler textureImageSampler);
std::vector<VkDescriptorSet> createDescriptorSets2d(
    VulkanCoreInfo& vulkanCoreInfo,
    VkDescriptorPool descriptorPool,
    VkDescriptorSetLayout descriptorSetLayout,
    std::vector<ImageInfo>& uiImageInfos,
    VkSampler textureImageSampler);