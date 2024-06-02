#pragma once

#include "VulkanTypes.hpp"
#include "Constants.hpp"
#include "Rendering/VertexBufferManager.hpp"

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
    std::vector<VkDescriptorSet> descriptorSets,
    CameraHandler cameraHandler,
    VertexBufferManager vertexBufferManager);

void createSyncObjects(
    VulkanCoreInfo* vulkanCoreInfo,
    std::vector<VkSemaphore>& imageAvailableSemaphores,
    std::vector<VkSemaphore>& renderFinishedSemaphores,
    std::vector<VkFence>& inFlightFences);