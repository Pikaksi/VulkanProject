#pragma once

#include <vector>

#include "Constants.hpp"
#include "Rendering/VertexBufferManager.hpp"
#include "vulkan/vulkan_core.h"

// All components derive from UIObject.
// To create a UIObject, create it with the given functions.
// If you edit the UIObject call updateUIObject. (Even if you change it right after creation.)
// Destroy the object with destroyUIObject.
struct UIManager
{
    glm::vec2 scalar; // change with changeExtent
    VkExtent2D extent; // change with changeExtent
    uint32_t uiMemoryPointer = 0;
    std::vector<Vertex2D> vertices;

    std::array<GpuMemoryBlock, MAX_FRAMES_IN_FLIGHT> gpuMemoryBlocks;

    void init(VulkanCoreInfo& vulkanCoreInfo, VkExtent2D extent);
    glm::vec2 getMousePositionScreenSpace();

    void changeExtent(VkExtent2D newExtent);
    void writeToBufferAndClear(uint32_t currentFrame);
    void cleanup(VulkanCoreInfo& vulkanCoreInfo);
};
