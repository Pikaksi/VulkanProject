#pragma once

#include "vulkan/vulkan_core.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "glm/mat4x4.hpp"

#include <vector>
#include <optional>

struct VulkanCoreInfo
{
    VkInstance instance;
    VkPhysicalDevice physicalDevice;
    VkDevice device;

    VkDebugUtilsMessengerEXT debugMessenger;

    VkQueue graphicsQueue;
    VkQueue presentQueue;

    GLFWwindow* window;
    VkSurfaceKHR surface;

    VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT; // default value
};

struct ImageInfo
{
    VkImage image;
    VkDeviceMemory memory;
    VkImageView view;
    uint32_t mipLevels;
};

struct SwapChainInfo
{
    VkSwapchainKHR swapChain;
    VkFormat imageFormat;
    VkExtent2D extent;
    std::vector<VkImageView> imageViews;
    std::vector<VkImage> images;

    ImageInfo depthImage = ImageInfo();
    VkFormat depthImageFormat;

    ImageInfo colorImage = ImageInfo();

    ImageInfo sunShadowImage = ImageInfo();
};

struct GraphicsPipelineInfo
{
    VkPipeline pipeline;
    VkPipelineLayout layout;
};

struct UniformBufferInfo
{
    VkBuffer buffer;
    VkDeviceMemory memory;
    void* mappingPointer;
};

struct CameraUniformBufferObject
{
    alignas(16) glm::mat4 camera;
    alignas(16) glm::mat4 sun;
};

struct PushConstant3d
{
    glm::vec3 chunkWorldLocation;
};
