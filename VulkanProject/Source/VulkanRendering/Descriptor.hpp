#pragma once

#include "VulkanTypes.hpp"

VkDescriptorSetLayout createDescriptorSetLayout(VulkanCoreInfo* vulkanCoreInfo);
VkDescriptorPool createDescriptorPool(VulkanCoreInfo* vulkanCoreInfo);
std::vector<VkDescriptorSet> createDescriptorSets(VulkanCoreInfo* vulkanCoreInfo,
    VkDescriptorPool descriptorPool,
    VkDescriptorSetLayout descriptorSetLayout,
    std::vector<UniformBufferInfo*> cameraUniformBuffers,
    ImageInfo* textureImage,
    VkSampler textureImageSampler);