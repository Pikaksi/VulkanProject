#pragma once

#include "VulkanTypes.hpp"
#include "Constants.hpp"

void drawFrame(
    VulkanCoreInfo* vulkanCoreInfo,
    SwapChainInfo* swapChainInfo,
    GraphicsPipelineInfo* GraphicsPipelineInfo,
    std::vector<UniformBufferInfo*> uniformBufferInfos,
    uint32_t& currentFrame,
    bool& framebufferResized,
    std::vector<VkCommandBuffer> commandBuffers,
    std::vector<VkSemaphore> imageAvailableSemaphores,
    std::vector<VkSemaphore> renderFinishedSemaphores,
    std::vector<VkFence> inFlightFences,
    CameraHandler cameraHandler);

void createSyncObjects(
    VulkanCoreInfo* vulkanCoreInfo,
    std::vector<VkSemaphore> imageAvailableSemaphores,
    std::vector<VkSemaphore> renderFinishedSemaphores,
    std::vector<VkFence> inFlightFences);