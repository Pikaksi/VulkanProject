#pragma once

#include "VulkanTypes.hpp"

VkDescriptorSetLayout createDescriptorSetLayout3d(VulkanCoreInfo& vulkanCoreInfo);
VkDescriptorSetLayout createDescriptorSetLayoutLod(VulkanCoreInfo& vulkanCoreInfo);
VkDescriptorSetLayout createDescriptorSetLayout2d(VulkanCoreInfo& vulkanCoreInfo);

VkDescriptorPool createDescriptorPool(VulkanCoreInfo& vulkanCoreInfo, uint32_t uiImageCount);

std::vector<VkDescriptorSet> createDescriptorSetsLod(VulkanCoreInfo& vulkanCoreInfo,
                                                    VkDescriptorPool descriptorPool,
                                                    VkDescriptorSetLayout descriptorSetLayout,
                                                    std::vector<UniformBufferInfo>& cameraUniformBuffers,
                                                    ImageInfo& sunShadowImage,
                                                    VkSampler sunShadowSampler);

std::vector<VkDescriptorSet> createDescriptorSets3d(VulkanCoreInfo& vulkanCoreInfo,
                                                    VkDescriptorPool descriptorPool,
                                                    VkDescriptorSetLayout descriptorSetLayout,
                                                    std::vector<UniformBufferInfo>& cameraUniformBuffers,
                                                    ImageInfo& textureImage,
                                                    VkSampler textureImageSampler,
                                                    ImageInfo& sunShadowImage,
                                                    VkSampler sunShadowSampler);
std::vector<VkDescriptorSet> createDescriptorSets2d(VulkanCoreInfo& vulkanCoreInfo,
                                                    VkDescriptorPool descriptorPool,
                                                    VkDescriptorSetLayout descriptorSetLayout,
                                                    std::vector<ImageInfo>& uiImageInfos,
                                                    VkSampler textureImageSampler);
