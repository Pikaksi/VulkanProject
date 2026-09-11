#include "sunShadows.hpp"

#include <iostream>
#include <array>

#include "VulkanTypes.hpp"
#include "ImageCreator.hpp"
#include "Commands.hpp"

void createShadowDepthImage(VulkanCoreInfo& vulkanCoreInfo,
                            SwapChainInfo& swapChainInfo,
                            VkCommandPool commandPool,
                            ImageInfo& sunShadowImage)
{
    createImageInfo(vulkanCoreInfo,
                    sunShadowImage,
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

    std::array<VkImageMemoryBarrier2, 1> outputBarriers{
        VkImageMemoryBarrier2{
                              .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
                              .srcStageMask = 0,
                              .srcAccessMask = 0,
                              .dstStageMask = 0,
                              .dstAccessMask = 0,
                              .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                              .newLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
                              .image = sunShadowImage.image,
                              .subresourceRange{.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT, .levelCount = 1, .layerCount = 1}}
    };
    VkDependencyInfo barrierDependencyInfo{.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
                                           .imageMemoryBarrierCount = outputBarriers.size(),
                                           .pImageMemoryBarriers = outputBarriers.data()};
    vkCmdPipelineBarrier2(commandBuffer, &barrierDependencyInfo);

    endSingleTimeCommands(vulkanCoreInfo, commandPool, commandBuffer);
}
