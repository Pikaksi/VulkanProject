#pragma once

#include "glm/mat4x4.hpp"

#include "VulkanTypes.hpp"
#include "Rendering/Vertex.hpp"

void createVertexBuffer(VulkanCoreInfo* vulkanCoreInfo, VkCommandPool commandPool, VkBuffer& buffer, VkDeviceMemory& memory, std::vector<Vertex>& vertices);
void createIndexBuffer(VulkanCoreInfo* vulkanCoreInfo, VkCommandPool commandPool, VkBuffer& buffer, VkDeviceMemory& memory, std::vector<uint32_t>& indices);
void copyBuffer(VulkanCoreInfo* vulkanCoreInfo, VkCommandPool commandPool, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkDeviceSize srcOffset, VkDeviceSize dstOffset);
void createBuffer(VulkanCoreInfo* vulkanCoreInfo, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
void createCameraUniformBuffers(VulkanCoreInfo* vulkanCoreInfo, std::vector<UniformBufferInfo*>& cameraUBOs);
void copyBufferToImage(VulkanCoreInfo* vulkanCoreInfo, VkCommandPool commandPool, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount);