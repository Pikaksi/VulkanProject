#pragma once

#include "DebugMenu.hpp"
#include "VulkanTypes.hpp"
#include "Constants.hpp"
#include "Rendering/VertexBufferManager.hpp"
#include "2dRendering/UIManager.hpp"
#include "CameraHandler.hpp"

struct FrameDrawInfo
{
    GraphicsPipelineInfo& pipeline3d;
    GraphicsPipelineInfo& pipelineLod;
    GraphicsPipelineInfo& pipelineSunShadow;
    GraphicsPipelineInfo& pipeline2d;

    std::vector<VkDescriptorSet>& descriptorSetsLod;
    std::vector<VkDescriptorSet>& descriptorSets3d;
    std::vector<VkDescriptorSet>& descriptorSets2d;
    std::vector<UniformBufferInfo>& uniformBufferInfos;

    uint32_t& currentFrame;
    bool& framebufferResized;

    std::vector<VkCommandBuffer>& commandBuffers;
    std::vector<VkSemaphore>& imageAvailableSemaphores;
    std::vector<VkSemaphore>& renderFinishedSemaphores;
    std::vector<VkFence>& inFlightFences;

    VkCommandPool commandPool;

    CameraHandler& cameraHandler;
    VertexBufferManager& vertexBufferManager;
    UIManager& uiManager;
    DebugMenu& debugMenu;
};

void drawFrame(VulkanCoreInfo& vulkanCoreInfo, SwapChainInfo& swapChainInfo, FrameDrawInfo& draw);

/*void drawFrame(
    VulkanCoreInfo& vulkanCoreInfo,
    SwapChainInfo& swapChainInfo,
    GraphicsPipelineInfo& graphicsPipelineInfo3d,
    GraphicsPipelineInfo& graphicsPipelineInfoSunShadow,
    GraphicsPipelineInfo& graphicsPipelineInfo2d,
    std::vector<VkDescriptorSet>& descriptorSets3d,
    std::vector<VkDescriptorSet>& descriptorSets2d,
    std::vector<UniformBufferInfo>& uniformBufferInfos,
    uint32_t& currentFrame,
    bool& framebufferResized,
    std::vector<VkCommandBuffer>& commandBuffers,
    std::vector<VkSemaphore>& imageAvailableSemaphores,
    std::vector<VkSemaphore>& renderFinishedSemaphores,
    std::vector<VkFence>& inFlightFences,
    VkCommandPool commandPool,
    CameraHandler& cameraHandler,
    VertexBufferManager& vertexBufferManager,
    UIManager& uIManager,
    DebugMenu& debugMenu);*/

void createSyncObjects(
    VulkanCoreInfo& vulkanCoreInfo,
    SwapChainInfo& swapChainInfo,
    std::vector<VkSemaphore>& imageAvailableSemaphores,
    std::vector<VkSemaphore>& renderFinishedSemaphores,
    std::vector<VkFence>& inFlightFences);
