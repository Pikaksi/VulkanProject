#pragma once

#include "DebugMenu.hpp"
#include "VulkanTypes.hpp"
#include "Constants.hpp"
#include "Rendering/VertexBufferManager.hpp"
#include "2dRendering/UIManager.hpp"
#include "CameraHandler.hpp"
#include "GPUMemoryBlock.hpp"
#include "drawCallRecorder.hpp"
#include "drawCallRecorder.hpp"

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

    ImageInfo sunShadowImage;

    std::array<std::array<DrawCallRecorder, 3>, MAX_FRAMES_IN_FLIGHT>& drawCallRecorders;
    std::vector<VkCommandBuffer>& commandBuffers;
    std::vector<VkSemaphore>& imageAvailableSemaphores;
    std::vector<VkSemaphore>& renderFinishedSemaphores;
    std::vector<VkFence>& inFlightFences;

    VkCommandPool commandPool;
    std::vector<QueryPoolInfo>& frameTimeQueryPools;

    CameraHandler& cameraHandler;
    VertexBufferManager& vertexBufferManager;
    UIManager& uiManager;
    DebugMenu& debugMenu;
};

void drawFrame(VulkanCoreInfo& vulkanCoreInfo, SwapChainInfo& swapChainInfo, FrameDrawInfo& draw);
void createSyncObjects(VulkanCoreInfo& vulkanCoreInfo,
                       SwapChainInfo& swapChainInfo,
                       std::vector<VkSemaphore>& imageAvailableSemaphores,
                       std::vector<VkSemaphore>& renderFinishedSemaphores,
                       std::vector<VkFence>& inFlightFences);
void createDrawCallBuffers(VulkanCoreInfo& vulkanCoreInfo,
                           std::array<std::array<DrawCallRecorder, 3>, MAX_FRAMES_IN_FLIGHT>& drawCallBuffers);
