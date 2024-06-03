#pragma once

#include "VulkanTypes.hpp"
#include "Constants.hpp"
#include "Rendering/VertexBufferManager.hpp"

void drawFrame(
    VulkanCoreInfo* vulkanCoreInfo,
    SwapChainInfo* swapChainInfo,
    GraphicsPipelineInfo* graphicsPipelineInfo3d,
    GraphicsPipelineInfo* graphicsPipelineInfo2d,
    std::vector<VkDescriptorSet> descriptorSets3d,
    std::vector<VkDescriptorSet> descriptorSets2d,
    std::vector<UniformBufferInfo*> uniformBufferInfos,
    uint32_t& currentFrame,
    bool& framebufferResized,
    std::vector<VkCommandBuffer> commandBuffers,
    std::vector<VkSemaphore> imageAvailableSemaphores,
    std::vector<VkSemaphore> renderFinishedSemaphores,
    std::vector<VkFence> inFlightFences,
    CameraHandler cameraHandler,
    VertexBufferManager vertexBufferManager);

void createSyncObjects(
    VulkanCoreInfo* vulkanCoreInfo,
    std::vector<VkSemaphore>& imageAvailableSemaphores,
    std::vector<VkSemaphore>& renderFinishedSemaphores,
    std::vector<VkFence>& inFlightFences);