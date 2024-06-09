#pragma once

#include <vector>
#include <array>

#include "VulkanRendering/VulkanTypes.hpp"
#include "VulkanRendering/Buffers.hpp"
#include "UIQuad.hpp"
#include "UIText.hpp"
#include "Constants.hpp"

class UIManager
{
public:
	std::vector<UIQuad> currentUIElements;
    std::vector<UIText> currentTextElements;

    bool uIRefreshNeeded = false;
    int updatedUIIndex = 0;

    std::array<VkBuffer, MAX_FRAMES_IN_FLIGHT> vertexBuffer;
    std::array<VkDeviceMemory, MAX_FRAMES_IN_FLIGHT> vertexBufferMemory;
    std::array<VkBuffer, MAX_FRAMES_IN_FLIGHT> indexBuffer;
    std::array<VkDeviceMemory, MAX_FRAMES_IN_FLIGHT> indexBufferMemory;
    std::array<uint32_t, MAX_FRAMES_IN_FLIGHT> indexCount;
    std::array<bool, MAX_FRAMES_IN_FLIGHT> hasElementsToRender;

    UIManager()
    {
        //currentUIElements.push_back(&UIQuad(-0.9, -0.9, 0.1f, 0.1f, 1.0f / 16.0f, 1.0f / 8.0f, 0.1, 0.2));
        //currentTextElements.push_back(&UIText(0.0f, 0.0f, 0.1f, "1234567890"));
    }

    void refreshUI(VulkanCoreInfo* vulkanCoreInfo, VkCommandPool commandPool, int screenWidth, int screenHeight);
    UIText* createUIText(float x, float y, float letterHeight, std::string text);
    void deleteUIText(UIText* uIText);
    void cleanUpSingleFrame(VulkanCoreInfo* vulkanCoreInfo, int index);
    void cleanUpAll(VulkanCoreInfo* vulkanCoreInfo);
};