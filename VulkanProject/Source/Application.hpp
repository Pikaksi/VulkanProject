#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

// #define STB_IMAGE_IMPLEMENTATION in main.cpp
#include <stb_image.h>

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <limits>
#include <array>
#include <optional>
#include <set>
#include <unordered_map>
#include <chrono>

#include "VulkanRendering/DeviceCreator.hpp"
#include "VulkanRendering/SwapChain.hpp"
#include "VulkanRendering/VulkanTypes.hpp"
#include "Rendering/VertexBufferManager.hpp"
#include "Rendering/ChunkRenderer.hpp"
#include "Rendering/VertexCreator.hpp"
#include "Rendering/Vertex.hpp"
#include "FilePathHandler.hpp"
#include "PlayerInputHandler.hpp"
#include "CameraHandler.hpp"
#include "World/WorldManager.hpp"

class Application
{
public:
    static Application& getInstance()
    {
        static Application application;
        return application;
    }
    void run();

private:
    Application()
    {
    }

    VulkanCoreInfo* vulkanCoreInfo;
    SwapChainInfo* swapChainInfo;

    VertexBufferManager vertexBufferManager;

    GraphicsPipelineInfo* graphicsPipelineInfo;

    VkCommandPool commandPool;

    ImageInfo* textureImage;
    VkSampler textureSampler;

    VkDescriptorSetLayout descriptorSetLayout;
    VkDescriptorPool descriptorPool;
    std::vector<VkDescriptorSet> descriptorSets;

    std::vector<UniformBufferInfo*> uniformBuffers;

    std::vector<VkCommandBuffer> commandBuffers;

    std::vector<UniformBufferInfo*> cameraUniformBuffers;

    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    uint32_t currentFrame = 0;

    ChunkRenderer chunkRenderer;
    CameraHandler cameraHandler;
    WorldManager worldManager;

    bool framebufferResized = false;

    static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
    void initGame();
    void initVulkan();
    void mainLoop();
    void gameMainLoop();
    void cleanup();
};