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

const int MAX_FRAMES_IN_FLIGHT = 2;

class Application
{
public:
    static Application& getInstance()
    {
        static Application application;
        return application;
    }
    void run();

    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
    void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);
    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

private:
    Application()
    {
    }

    VulkanCoreInfo* vulkanCoreInfo;
    SwapChainInfo* swapChainInfo;

    VertexBufferManager vertexBufferManager;

    VkRenderPass renderPass;
    VkDescriptorSetLayout descriptorSetLayout;
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;

    VkCommandPool commandPool;

    VkImage colorImage;
    VkDeviceMemory colorImageMemory;
    VkImageView colorImageView;

    VkImage depthImage;
    VkDeviceMemory depthImageMemory;
    VkImageView depthImageView;

    uint32_t mipLevels;
    VkImage textureImage;
    VkDeviceMemory textureImageMemory;
    VkImageView textureImageView;

    VkSampler textureSampler;

    std::vector<VkBuffer> uniformBuffers;
    std::vector<VkDeviceMemory> uniformBuffersMemory;
    std::vector<void*> uniformBuffersMapped;

    VkDescriptorPool descriptorPool;
    std::vector<VkDescriptorSet> descriptorSets;

    std::vector<VkCommandBuffer> commandBuffers;

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
    void createDescriptorSetLayout();
    void createGraphicsPipeline();
    void createCommandPool();
    bool hasStencilComponent(VkFormat format);
    void generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);
    void createTextureSampler();
    void createUniformBuffers();
    void createDescriptorPool();
    void createDescriptorSets();
    VkCommandBuffer beginSingleTimeCommands();
    void endSingleTimeCommands(VkCommandBuffer commandBuffer);
    void createCommandBuffers();
    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
    void createSyncObjects();
    void updateUniformBuffer(uint32_t currentImage);
    void drawFrame();
    VkShaderModule createShaderModule(const std::vector<char>& code);
    static std::vector<char> readFile(const std::string& filename);
};