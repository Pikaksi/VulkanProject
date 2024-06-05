#pragma once

#include <vector>

#include "VulkanRendering/VulkanTypes.hpp"
#include "VulkanRendering/Buffers.hpp"
#include "UIQuad.hpp"

class UIManager
{
public:
	std::vector<UIQuad> currentUIElements;

    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;
    uint32_t indexCount;

    UIManager()
    {
        currentUIElements.push_back(UIQuad(-0.9, -0.9, 0.1, 0.2));
    }

    void refreshUI(VulkanCoreInfo* vulkanCoreInfo, VkCommandPool commandPool, int screenWidth, int screenHeight);
    void cleanUpBuffers(VulkanCoreInfo* vulkanCoreInfo);
};