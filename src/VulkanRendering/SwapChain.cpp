#include "SwapChain.hpp"

#include <algorithm>
#include <cassert>
#include <stdexcept>
#include <array>
#include <iostream>

#include "Commands.hpp"
#include "DeviceCreator.hpp"
#include "ImageCreator.hpp"
#include "VulkanUtilities.hpp"
#include "vulkan/vulkan_core.h"

SwapChainSupportDetails querySwapChainSupport(VulkanCoreInfo& vulkanCoreInfo)
{
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
        vulkanCoreInfo.physicalDevice, vulkanCoreInfo.surface, &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(vulkanCoreInfo.physicalDevice, vulkanCoreInfo.surface, &formatCount, nullptr);

    if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(
            vulkanCoreInfo.physicalDevice, vulkanCoreInfo.surface, &formatCount, details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(
        vulkanCoreInfo.physicalDevice, vulkanCoreInfo.surface, &presentModeCount, nullptr);

    if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(
            vulkanCoreInfo.physicalDevice, vulkanCoreInfo.surface, &presentModeCount, details.presentModes.data());
    }

    return details;
}

VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
            availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }

    std::cout << "Could not find wanted format VK_FORMAT_B8G8R8A8_SRGB. Might cause unwanted behaviour" << std::endl;
    return availableFormats[0];
}

VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{

    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D chooseSwapExtent(VulkanCoreInfo& vulkanCoreInfo, const VkSurfaceCapabilitiesKHR& capabilities)
{
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    }
    else {
        int width, height;
        glfwGetFramebufferSize(vulkanCoreInfo.window, &width, &height);

        VkExtent2D actualExtent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};

        actualExtent.width =
            std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height =
            std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
}

VkFormat findDepthFormat(VulkanCoreInfo& vulkanCoreInfo)
{
    return findSupportedFormat(vulkanCoreInfo,
                               {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
                               VK_IMAGE_TILING_OPTIMAL,
                               VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

void createSwapChain(VulkanCoreInfo& vulkanCoreInfo, SwapChainInfo& swapChainInfo, VkCommandPool commandPool)
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
    uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

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

    swapChainInfo.imageViews.resize(swapChainInfo.images.size());
    for (uint32_t i = 0; i < swapChainInfo.images.size(); i++) {
        fillImageView(vulkanCoreInfo,
                      swapChainInfo.images[i],
                      swapChainInfo.imageViews[i],
                      swapChainInfo.imageFormat,
                      VK_IMAGE_ASPECT_COLOR_BIT,
                      1,
                      1,
                      VK_IMAGE_VIEW_TYPE_2D);
    }

    swapChainInfo.depthImageFormat = findDepthFormat(vulkanCoreInfo);
    createImageInfo(vulkanCoreInfo,
                    swapChainInfo.depthImage,
                    swapChainInfo.extent.width,
                    swapChainInfo.extent.height,
                    1,
                    vulkanCoreInfo.msaaSamples,
                    swapChainInfo.depthImageFormat,
                    VK_IMAGE_TILING_OPTIMAL,
                    VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                    VK_IMAGE_ASPECT_DEPTH_BIT,
                    1,
                    VK_IMAGE_VIEW_TYPE_2D);

    createImageInfo(vulkanCoreInfo,
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

    createImageInfo(vulkanCoreInfo,
                    swapChainInfo.sunShadowImage,
                    2048,
                    2048,
                    1,
                    VK_SAMPLE_COUNT_1_BIT,
                    swapChainInfo.depthImageFormat,
                    VK_IMAGE_TILING_OPTIMAL,
                    VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                    VK_IMAGE_ASPECT_DEPTH_BIT,
                    1,
                    VK_IMAGE_VIEW_TYPE_2D);

    VkCommandBuffer commandBuffer = beginSingleTimeCommands(vulkanCoreInfo, commandPool);

    std::array<VkImageMemoryBarrier2, 3> outputBarriers{
        VkImageMemoryBarrier2{
                              .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
                              .srcStageMask = 0,
                              .srcAccessMask = 0,
                              .dstStageMask = 0,
                              .dstAccessMask = 0,
                              .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                              .newLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
                              .image = swapChainInfo.colorImage.image,
                              .subresourceRange{.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .levelCount = 1, .layerCount = 1}},
        VkImageMemoryBarrier2{
                              .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
                              .srcStageMask = 0,
                              .srcAccessMask = 0,
                              .dstStageMask = 0,
                              .dstAccessMask = 0,
                              .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                              .newLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
                              .image = swapChainInfo.depthImage.image,
                              .subresourceRange{.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT, .levelCount = 1, .layerCount = 1}},
        VkImageMemoryBarrier2{
                              .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
                              .srcStageMask = 0,
                              .srcAccessMask = 0,
                              .dstStageMask = 0,
                              .dstAccessMask = 0,
                              .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                              .newLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
                              .image = swapChainInfo.sunShadowImage.image,
                              .subresourceRange{.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT, .levelCount = 1, .layerCount = 1}}
    };
    VkDependencyInfo barrierDependencyInfo{.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
                                           .imageMemoryBarrierCount = outputBarriers.size(),
                                           .pImageMemoryBarriers = outputBarriers.data()};
    vkCmdPipelineBarrier2(commandBuffer, &barrierDependencyInfo);

    endSingleTimeCommands(vulkanCoreInfo, commandPool, commandBuffer);

    /*VkAttachmentDescription colorAttachment{};
    colorAttachment.format = swapChainInfo.imageFormat;
    colorAttachment.samples = vulkanCoreInfo.msaaSamples;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription depthAttachment{};
    depthAttachment.format = findDepthFormat(vulkanCoreInfo);
    depthAttachment.samples = vulkanCoreInfo.msaaSamples;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription colorAttachmentResolve{};
    colorAttachmentResolve.format = swapChainInfo.imageFormat;
    colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef{};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference colorAttachmentResolveRef{};
    colorAttachmentResolveRef.attachment = 2;
    colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;
    subpass.pResolveAttachments = &colorAttachmentResolveRef;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
    VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT; dependency.srcAccessMask = 0; dependency.dstStageMask =
    VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT; dependency.dstAccessMask
    = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    std::array<VkAttachmentDescription, 3> attachments = { colorAttachment, depthAttachment, colorAttachmentResolve };
    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    VkRenderPass renderPass;
    if (vkCreateRenderPass(vulkanCoreInfo.device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
        throw std::runtime_error("failed to create render pass!");
    }*/
}

void cleanupSwapChain(VulkanCoreInfo& vulkanCoreInfo, SwapChainInfo& swapChainInfo)
{

    vkDestroyImageView(vulkanCoreInfo.device, swapChainInfo.depthImage.view, nullptr);
    vkDestroyImage(vulkanCoreInfo.device, swapChainInfo.depthImage.image, nullptr);
    vkFreeMemory(vulkanCoreInfo.device, swapChainInfo.depthImage.memory, nullptr);

    vkDestroyImageView(vulkanCoreInfo.device, swapChainInfo.colorImage.view, nullptr);
    vkDestroyImage(vulkanCoreInfo.device, swapChainInfo.colorImage.image, nullptr);
    vkFreeMemory(vulkanCoreInfo.device, swapChainInfo.colorImage.memory, nullptr);

    vkDestroyImageView(vulkanCoreInfo.device, swapChainInfo.sunShadowImage.view, nullptr);
    vkDestroyImage(vulkanCoreInfo.device, swapChainInfo.sunShadowImage.image, nullptr);
    vkFreeMemory(vulkanCoreInfo.device, swapChainInfo.sunShadowImage.memory, nullptr);

    for (auto imageView : swapChainInfo.imageViews) {
        vkDestroyImageView(vulkanCoreInfo.device, imageView, nullptr);
    }
    vkDestroySwapchainKHR(vulkanCoreInfo.device, swapChainInfo.swapChain, nullptr);
}

void recreateSwapChain(VulkanCoreInfo& vulkanCoreInfo, SwapChainInfo& swapChainInfo, VkCommandPool commandPool)
{
    int width = 0, height = 0;
    glfwGetFramebufferSize(vulkanCoreInfo.window, &width, &height);
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(vulkanCoreInfo.window, &width, &height);
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(vulkanCoreInfo.device);

    cleanupSwapChain(vulkanCoreInfo, swapChainInfo);

    createSwapChain(vulkanCoreInfo, swapChainInfo, commandPool);
}
