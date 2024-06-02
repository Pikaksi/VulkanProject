#pragma once

#include "glm/mat4x4.hpp"

#include "VulkanTypes.hpp"

void copyBuffer(VulkanCoreInfo* vulkanCoreInfo, VkCommandPool commandPool, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
void createBuffer(VulkanCoreInfo* vulkanCoreInfo, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
void createCameraUniformBuffers(VulkanCoreInfo* vulkanCoreInfo, std::vector<UniformBufferInfo*>& cameraUBOs);
void copyBufferToImage(VulkanCoreInfo* vulkanCoreInfo, VkCommandPool commandPool, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);