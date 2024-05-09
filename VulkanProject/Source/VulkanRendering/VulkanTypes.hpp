#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

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

	VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;
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
	std::vector<VkImage> swapChainImages;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
	std::vector<VkImageView> swapChainImageViews;
	std::vector<VkFramebuffer> swapChainFramebuffers;

	VkRenderPass renderPass;

	ImageInfo* colorImage;
	ImageInfo* depthImage;
};
