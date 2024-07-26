#include "SwapChain.hpp"

#include <algorithm>
#include <stdexcept>
#include <array>
#include <iostream>

#include "DeviceCreator.hpp"
#include "ImageCreator.hpp"
#include "VulkanUtilities.hpp"
#include "RenderPass.hpp"

SwapChainSupportDetails querySwapChainSupport(VulkanCoreInfo& vulkanCoreInfo) {
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vulkanCoreInfo.physicalDevice, vulkanCoreInfo.surface, &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(vulkanCoreInfo.physicalDevice, vulkanCoreInfo.surface, &formatCount, nullptr);

    if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(vulkanCoreInfo.physicalDevice, vulkanCoreInfo.surface, &formatCount, details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(vulkanCoreInfo.physicalDevice, vulkanCoreInfo.surface, &presentModeCount, nullptr);

    if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(vulkanCoreInfo.physicalDevice, vulkanCoreInfo.surface, &presentModeCount, details.presentModes.data());
    }

    return details;
}

VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D chooseSwapExtent(VulkanCoreInfo& vulkanCoreInfo, const VkSurfaceCapabilitiesKHR& capabilities) {
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    }
    else {
        int width, height;
        glfwGetFramebufferSize(vulkanCoreInfo.window, &width, &height);

        VkExtent2D actualExtent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
}

void createSwapChainImageViews(VulkanCoreInfo& vulkanCoreInfo, SwapChainInfo& swapChainInfo) {
    swapChainInfo.imageViews.resize(swapChainInfo.images.size());

    for (uint32_t i = 0; i < swapChainInfo.images.size(); i++) {
        fillImageView(vulkanCoreInfo, swapChainInfo.images[i], swapChainInfo.imageViews[i], swapChainInfo.imageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1, 1, VK_IMAGE_VIEW_TYPE_2D);
    }
}

void createColorResources(VulkanCoreInfo& vulkanCoreInfo, SwapChainInfo& swapChainInfo) {
    createImageInfo(
        vulkanCoreInfo, 
        swapChainInfo.colorImage,
        swapChainInfo.extent.width, 
        swapChainInfo.extent.height, 
        1, 
        vulkanCoreInfo.msaaSamples, 
        swapChainInfo.imageFormat,
        VK_IMAGE_TILING_OPTIMAL, 
        VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, 
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        VK_IMAGE_ASPECT_COLOR_BIT,
        1,
        VK_IMAGE_VIEW_TYPE_2D);
}

VkFormat findDepthFormat(VulkanCoreInfo& vulkanCoreInfo) {
    return findSupportedFormat(
        vulkanCoreInfo,
        { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
    );
}

void createDepthResources(VulkanCoreInfo& vulkanCoreInfo, SwapChainInfo& swapChainInfo) {
    createImageInfo(
        vulkanCoreInfo,
        swapChainInfo.depthImage, 
        swapChainInfo.extent.width, 
        swapChainInfo.extent.height, 
        1, 
        vulkanCoreInfo.msaaSamples, 
        findDepthFormat(vulkanCoreInfo),
        VK_IMAGE_TILING_OPTIMAL, 
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, 
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        VK_IMAGE_ASPECT_DEPTH_BIT,
        1,
        VK_IMAGE_VIEW_TYPE_2D);
}

void createFramebuffers(VulkanCoreInfo& vulkanCoreInfo, SwapChainInfo& swapChainInfo) {
    swapChainInfo.framebuffers.resize(swapChainInfo.imageViews.size());

    for (size_t i = 0; i < swapChainInfo.imageViews.size(); i++) {
        std::array<VkImageView, 3> attachments = {
            swapChainInfo.colorImage.view,
            swapChainInfo.depthImage.view,
            swapChainInfo.imageViews[i]
        };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = swapChainInfo.renderPass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = swapChainInfo.extent.width;
        framebufferInfo.height = swapChainInfo.extent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(vulkanCoreInfo.device, &framebufferInfo, nullptr, &swapChainInfo.framebuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
}

void createSwapChain(VulkanCoreInfo& vulkanCoreInfo, SwapChainInfo& swapChainInfo)
{
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(vulkanCoreInfo);

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = chooseSwapExtent(vulkanCoreInfo, swapChainSupport.capabilities);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = vulkanCoreInfo.surface;

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = findQueueFamilies(vulkanCoreInfo);
    uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

    if (indices.graphicsFamily != indices.presentFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;

    if (vkCreateSwapchainKHR(vulkanCoreInfo.device, &createInfo, nullptr, &swapChainInfo.swapChain) != VK_SUCCESS) {
        throw std::runtime_error("failed to create swap chain!");
    }

    vkGetSwapchainImagesKHR(vulkanCoreInfo.device, swapChainInfo.swapChain, &imageCount, nullptr);
    swapChainInfo.images.resize(imageCount);
    vkGetSwapchainImagesKHR(vulkanCoreInfo.device, swapChainInfo.swapChain, &imageCount, swapChainInfo.images.data());

    swapChainInfo.imageFormat = surfaceFormat.format;
    swapChainInfo.extent = extent;

    swapChainInfo.renderPass = createRenderPass(vulkanCoreInfo, swapChainInfo);

    createSwapChainImageViews(vulkanCoreInfo, swapChainInfo);
    createColorResources(vulkanCoreInfo, swapChainInfo);
    createDepthResources(vulkanCoreInfo, swapChainInfo);
    createFramebuffers(vulkanCoreInfo, swapChainInfo);
}

void cleanupSwapChain(VulkanCoreInfo& vulkanCoreInfo, SwapChainInfo& swapChainInfo) {

    vkDestroyImageView(vulkanCoreInfo.device, swapChainInfo.depthImage.view, nullptr);
    vkDestroyImage(vulkanCoreInfo.device, swapChainInfo.depthImage.image, nullptr);
    vkFreeMemory(vulkanCoreInfo.device, swapChainInfo.depthImage.memory, nullptr);

    vkDestroyImageView(vulkanCoreInfo.device, swapChainInfo.colorImage.view, nullptr);
    vkDestroyImage(vulkanCoreInfo.device, swapChainInfo.colorImage.image, nullptr);
    vkFreeMemory(vulkanCoreInfo.device, swapChainInfo.colorImage.memory, nullptr);

    for (auto framebuffer : swapChainInfo.framebuffers) {
        vkDestroyFramebuffer(vulkanCoreInfo.device, framebuffer, nullptr);
    }

    for (auto imageView : swapChainInfo.imageViews) {
        vkDestroyImageView(vulkanCoreInfo.device, imageView, nullptr);
    }
    vkDestroyRenderPass(vulkanCoreInfo.device, swapChainInfo.renderPass, nullptr);
    vkDestroySwapchainKHR(vulkanCoreInfo.device, swapChainInfo.swapChain, nullptr);
}

void recreateSwapChain(VulkanCoreInfo& vulkanCoreInfo, SwapChainInfo& swapChainInfo) {
    int width = 0, height = 0;
    glfwGetFramebufferSize(vulkanCoreInfo.window, &width, &height);
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(vulkanCoreInfo.window, &width, &height);
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(vulkanCoreInfo.device);

    cleanupSwapChain(vulkanCoreInfo, swapChainInfo);

    createSwapChain(vulkanCoreInfo, swapChainInfo);
}