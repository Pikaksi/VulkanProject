#include <stdexcept>
#include <utility>
#include <vector>
#include <array>

#include "FrameDrawer.hpp"
#include "SwapChain.hpp"
#include "CameraHandler.hpp"
#include "VulkanTypes.hpp"
#include "World/Chunk.hpp"
#include "vulkan/vulkan_core.h"

void createSyncObjects(VulkanCoreInfo& vulkanCoreInfo,
                       std::vector<VkSemaphore>& imageAvailableSemaphores,
                       std::vector<VkSemaphore>& renderFinishedSemaphores,
                       std::vector<VkFence>& inFlightFences)
{
    imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateSemaphore(vulkanCoreInfo.device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) !=
                VK_SUCCESS ||
            vkCreateSemaphore(vulkanCoreInfo.device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) !=
                VK_SUCCESS ||
            vkCreateFence(vulkanCoreInfo.device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create synchronization objects for a frame!");
        }
    }
}

void updateUniformBuffer(uint32_t currentFrame,
                         std::vector<UniformBufferInfo>& uniformBufferInfos,
                         CameraHandler& cameraHandler,
                         VkExtent2D swapChainExtent)
{
    CameraUniformBufferObject ubo;
    cameraHandler.getCameraMatrix(swapChainExtent, ubo);

    memcpy(uniformBufferInfos[currentFrame].mappingPointer, &ubo, sizeof(ubo));
}

struct ChunkCenterOffsets
{
    glm::ivec3 top;
    glm::ivec3 bottom;
    glm::ivec3 right;
    glm::ivec3 left;
};

void getChunkCenterOffsets(ChunkCenterOffsets& chunkCenterOffsets, ViewingFrustumNormals& viewingFrustumNormals)
{
    chunkCenterOffsets.top = {
        viewingFrustumNormals.top.x > 0.0f ? -viewingFrustumSafetyOffset : CHUNK_SIZE + viewingFrustumSafetyOffset,
        viewingFrustumNormals.top.y > 0.0f ? -viewingFrustumSafetyOffset : CHUNK_SIZE + viewingFrustumSafetyOffset,
        viewingFrustumNormals.top.z > 0.0f ? -viewingFrustumSafetyOffset : CHUNK_SIZE + viewingFrustumSafetyOffset};
    chunkCenterOffsets.bottom = {
        viewingFrustumNormals.bottom.x > 0.0f ? -viewingFrustumSafetyOffset : CHUNK_SIZE + viewingFrustumSafetyOffset,
        viewingFrustumNormals.bottom.y > 0.0f ? -viewingFrustumSafetyOffset : CHUNK_SIZE + viewingFrustumSafetyOffset,
        viewingFrustumNormals.bottom.z > 0.0f ? -viewingFrustumSafetyOffset : CHUNK_SIZE + viewingFrustumSafetyOffset};
    chunkCenterOffsets.right = {
        viewingFrustumNormals.right.x > 0.0f ? -viewingFrustumSafetyOffset : CHUNK_SIZE + viewingFrustumSafetyOffset,
        viewingFrustumNormals.right.y > 0.0f ? -viewingFrustumSafetyOffset : CHUNK_SIZE + viewingFrustumSafetyOffset,
        viewingFrustumNormals.right.z > 0.0f ? -viewingFrustumSafetyOffset : CHUNK_SIZE + viewingFrustumSafetyOffset};
    chunkCenterOffsets.left = {
        viewingFrustumNormals.left.x > 0.0f ? -viewingFrustumSafetyOffset : CHUNK_SIZE + viewingFrustumSafetyOffset,
        viewingFrustumNormals.left.y > 0.0f ? -viewingFrustumSafetyOffset : CHUNK_SIZE + viewingFrustumSafetyOffset,
        viewingFrustumNormals.left.z > 0.0f ? -viewingFrustumSafetyOffset : CHUNK_SIZE + viewingFrustumSafetyOffset};
}

bool chunkIsInViewingFrustum(glm::vec3& cameraLocation,
                             glm::ivec3& chunkLocation,
                             ChunkCenterOffsets& chunkCenterOffsets,
                             ViewingFrustumNormals& viewingFrustumNormals)
{
    // glm::vec3 toChunkTop = static_cast<glm::vec3>(chunkLocation/* + chunkCenterOffsets.top*/) - cameraLocation;
    // std::cout << "to chunk: " << toChunkTop.x << " " << toChunkTop.y << " " << toChunkTop.z << " frustum normal " <<
    // viewingFrustumNormals.top.x << " " << viewingFrustumNormals.top.y << " " << viewingFrustumNormals.top.z << "\n";
    return glm::dot(static_cast<glm::vec3>(chunkLocation * CHUNK_SIZE + chunkCenterOffsets.top) - cameraLocation,
                    viewingFrustumNormals.top) < 0.0f &&
           glm::dot(static_cast<glm::vec3>(chunkLocation * CHUNK_SIZE + chunkCenterOffsets.bottom) - cameraLocation,
                    viewingFrustumNormals.bottom) < 0.0f &&
           glm::dot(static_cast<glm::vec3>(chunkLocation * CHUNK_SIZE + chunkCenterOffsets.right) - cameraLocation,
                    viewingFrustumNormals.right) < 0.0f &&
           glm::dot(static_cast<glm::vec3>(chunkLocation * CHUNK_SIZE + chunkCenterOffsets.left) - cameraLocation,
                    viewingFrustumNormals.left) < 0.0f;
}

void recordCommandBuffer(VulkanCoreInfo& vulkanCoreInfo,
                         SwapChainInfo& swapChainInfo,
                         GraphicsPipelineInfo& graphicsPipelineInfo3d,
                         GraphicsPipelineInfo& graphicsPipelineInfoSunShadow,
                         GraphicsPipelineInfo& graphicsPipelineInfo2d,
                         VkDescriptorSet descriptorSet3d,
                         VkDescriptorSet descriptorSet2d,
                         VkCommandBuffer commandBuffer,
                         uint32_t swapChainImageIndex,
                         uint32_t currentFrame,
                         VertexBufferManager& vertexBufferManager,
                         CameraHandler& cameraHandler)
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    VkBuffer worldVertexBuffer;
    std::vector<WorldDrawCallData> worldDrawCallData;
    VkBuffer worldIndexBuffer;
    vertexBufferManager.getWorldGeometryForRendering(worldVertexBuffer, worldDrawCallData, worldIndexBuffer);

    // ---------------- SUN SHADOW PASS ----------------

    {
        VkImageMemoryBarrier2 presentImageToAttachmentBarrier{
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .srcStageMask = VK_PIPELINE_STAGE_2_NONE,
            .srcAccessMask = 0,
            .dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
            .dstAccessMask = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
            .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .newLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
            .image = swapChainInfo.sunShadowImage.image,
            .subresourceRange{.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT, .levelCount = 1, .layerCount = 1}
        };
        VkDependencyInfo barrierDependencyInfo{.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
                                               .imageMemoryBarrierCount = 1,
                                               .pImageMemoryBarriers = &presentImageToAttachmentBarrier};
        vkCmdPipelineBarrier2(commandBuffer, &barrierDependencyInfo);
    }

    VkRenderingAttachmentInfo sunShadowDepthAttachmentInfo{};
    sunShadowDepthAttachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    sunShadowDepthAttachmentInfo.imageView = swapChainInfo.sunShadowImage.view;
    sunShadowDepthAttachmentInfo.imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL;
    sunShadowDepthAttachmentInfo.clearValue = VkClearValue{
        .depthStencil = VkClearDepthStencilValue{.depth = 1.0f, .stencil = 0}
    };
    sunShadowDepthAttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    sunShadowDepthAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

    VkRenderingInfo sunShadowRenderingInfo{};
    sunShadowRenderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
    sunShadowRenderingInfo.colorAttachmentCount = 0;
    sunShadowRenderingInfo.pColorAttachments = nullptr;
    sunShadowRenderingInfo.pDepthAttachment = &sunShadowDepthAttachmentInfo;
    sunShadowRenderingInfo.renderArea = VkRect2D{
        .offset = {0, 0},
        .extent{.width = 2048, .height = 2048}
    };
    sunShadowRenderingInfo.layerCount = 1;

    vkCmdBeginRendering(commandBuffer, &sunShadowRenderingInfo);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipelineInfoSunShadow.pipeline);

    vkCmdSetDepthBias(commandBuffer, /*constant*/ 2.0f, /*clamp*/ 0.0f, /*slope*/ 6.0f);

    for (size_t i = 0; i < worldDrawCallData.size(); i++) {
        WorldDrawCallData drawCallData = worldDrawCallData[i];

        VkBuffer vertexBuffers[] = {worldVertexBuffer};

        VkDeviceSize offsets[] = {drawCallData.memoryLocation};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

        vkCmdBindIndexBuffer(commandBuffer, worldIndexBuffer, 0, VK_INDEX_TYPE_UINT32);

        vkCmdBindDescriptorSets(commandBuffer,
                                VK_PIPELINE_BIND_POINT_GRAPHICS,
                                graphicsPipelineInfo3d.layout,
                                0,
                                1,
                                &descriptorSet3d,
                                0,
                                nullptr);

        PushConstant3d pushConstant = {
            {0.0f, 10.0f, 0.0f}
        };
        vkCmdPushConstants(commandBuffer,
                           graphicsPipelineInfo3d.layout,
                           VK_SHADER_STAGE_VERTEX_BIT,
                           0,
                           sizeof(PushConstant3d),
                           &pushConstant);

        // get index count by multiplying vertex count by 1.5
        vkCmdDrawIndexed(commandBuffer, drawCallData.dataCount / 2 * 3, 1, 0, 0, 0);
    }

    vkCmdEndRendering(commandBuffer);

    {
        VkImageMemoryBarrier2 depthImageToReadOnlyBarrier{
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .srcStageMask = VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT,
            .srcAccessMask = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
            .dstStageMask = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
            .dstAccessMask = VK_ACCESS_2_SHADER_SAMPLED_READ_BIT,
            .oldLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
            .newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            .image = swapChainInfo.sunShadowImage.image,
            .subresourceRange{.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT, .levelCount = 1, .layerCount = 1}
        };

        VkDependencyInfo barrierDependencyInfo{VK_STRUCTURE_TYPE_DEPENDENCY_INFO};
        barrierDependencyInfo.imageMemoryBarrierCount = 1;
        barrierDependencyInfo.pImageMemoryBarriers = &depthImageToReadOnlyBarrier;

        vkCmdPipelineBarrier2(commandBuffer, &barrierDependencyInfo);
    }

    // ---------------- MAIN PASS----------------

    VkRenderingAttachmentInfo colorAttachmentInfo{};
    colorAttachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    colorAttachmentInfo.imageView = swapChainInfo.colorImage.view;
    colorAttachmentInfo.imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL;
    colorAttachmentInfo.clearValue = VkClearValue{.color = VkClearColorValue{.float32 = {0.2, 0.1, 0.7, 1.0}}};
    colorAttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE; // VK_ATTACHMENT_STORE_OP_STORE

    colorAttachmentInfo.resolveImageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL;
    colorAttachmentInfo.resolveMode = VK_RESOLVE_MODE_AVERAGE_BIT;
    colorAttachmentInfo.resolveImageView = swapChainInfo.imageViews[swapChainImageIndex];

    VkRenderingAttachmentInfo depthAttachmentInfo{};
    depthAttachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    depthAttachmentInfo.imageView = swapChainInfo.depthImage.view;
    depthAttachmentInfo.imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL;
    depthAttachmentInfo.clearValue = VkClearValue{
        .depthStencil = VkClearDepthStencilValue{.depth = 1.0f, .stencil = 0}
    };
    depthAttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

    VkRenderingInfo renderingInfo{};
    renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
    renderingInfo.colorAttachmentCount = 1;
    renderingInfo.pColorAttachments = &colorAttachmentInfo;
    renderingInfo.pDepthAttachment = &depthAttachmentInfo;
    renderingInfo.renderArea = VkRect2D{
        .extent{.width = swapChainInfo.extent.width, .height = swapChainInfo.extent.height}
    };
    renderingInfo.layerCount = 1;

    {
        VkImageMemoryBarrier2 presentImageToAttachmentBarrier{
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .srcStageMask = VK_PIPELINE_STAGE_2_NONE,
            .srcAccessMask = 0,
            .dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
            .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .newLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
            .image = swapChainInfo.images[swapChainImageIndex],
            .subresourceRange{.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .levelCount = 1, .layerCount = 1}
        };
        VkDependencyInfo barrierDependencyInfo{.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
                                               .imageMemoryBarrierCount = 1,
                                               .pImageMemoryBarriers = &presentImageToAttachmentBarrier};
        vkCmdPipelineBarrier2(commandBuffer, &barrierDependencyInfo);
    }

    vkCmdBeginRendering(commandBuffer, &renderingInfo);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)swapChainInfo.extent.width;
    viewport.height = (float)swapChainInfo.extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = swapChainInfo.extent;
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipelineInfo3d.pipeline);

    ViewingFrustumNormals viewingFrustumNormals;
    cameraHandler.getViewingFrustumNormals(swapChainInfo.extent, viewingFrustumNormals);
    ChunkCenterOffsets chunkCenterOffsets;
    getChunkCenterOffsets(chunkCenterOffsets, viewingFrustumNormals);

    // Render world.

    for (int i = 0; i < worldDrawCallData.size(); i++) {
        WorldDrawCallData drawCallData = worldDrawCallData[i];

        if (!chunkIsInViewingFrustum(
                cameraHandler.position, drawCallData.chunkLocation, chunkCenterOffsets, viewingFrustumNormals)) {
            continue;
        }

        VkBuffer vertexBuffers[] = {worldVertexBuffer};

        VkDeviceSize offsets[] = {drawCallData.memoryLocation};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

        vkCmdBindIndexBuffer(commandBuffer, worldIndexBuffer, 0, VK_INDEX_TYPE_UINT32);

        vkCmdBindDescriptorSets(commandBuffer,
                                VK_PIPELINE_BIND_POINT_GRAPHICS,
                                graphicsPipelineInfo3d.layout,
                                0,
                                1,
                                &descriptorSet3d,
                                0,
                                nullptr);

        PushConstant3d pushConstant = {
            {0.0f, 10.0f, 0.0f}
        };
        vkCmdPushConstants(commandBuffer,
                           graphicsPipelineInfo3d.layout,
                           VK_SHADER_STAGE_VERTEX_BIT,
                           0,
                           sizeof(PushConstant3d),
                           &pushConstant);

        // get index count by multiplying vertex count by 1.5
        vkCmdDrawIndexed(commandBuffer, drawCallData.dataCount / 2 * 3, 1, 0, 0, 0);
    }

    // Render UI.
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipelineInfo2d.pipeline);

    auto startingTime = std::chrono::high_resolution_clock::now();

    VkBuffer uiVertexBuffer;
    std::vector<VkDeviceSize> uiVertexOffsets;
    std::vector<uint32_t> uiBatchVertexCounts;
    VkBuffer uiIndexBuffer;
    vertexBufferManager.getUIGeometryForRendering(uiVertexBuffer, uiVertexOffsets, uiBatchVertexCounts, uiIndexBuffer);
    auto endingTime = std::chrono::high_resolution_clock::now();
    auto timeTaken = std::chrono::duration_cast<std::chrono::nanoseconds>(endingTime - startingTime).count();
    // std::cout << "time taken to render UI in nanoseconds is " << timeTaken << "\n";

    for (int i = 0; i < uiVertexOffsets.size(); i++) {
        VkBuffer vertexBuffers[] = {uiVertexBuffer};

        VkDeviceSize offsets[] = {uiVertexOffsets[i]};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

        vkCmdBindIndexBuffer(commandBuffer, uiIndexBuffer, 0, VK_INDEX_TYPE_UINT32);

        vkCmdBindDescriptorSets(commandBuffer,
                                VK_PIPELINE_BIND_POINT_GRAPHICS,
                                graphicsPipelineInfo2d.layout,
                                0,
                                1,
                                &descriptorSet2d,
                                0,
                                nullptr);

        vkCmdDrawIndexed(commandBuffer, uiBatchVertexCounts[i] / 2 * 3, 1, 0, 0, 0);
    }

    vkCmdEndRendering(commandBuffer);

    VkImageMemoryBarrier2 presentImageLayoutSwapBarrier{
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
        .srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
        .srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
        .dstStageMask = VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT,
        .dstAccessMask = 0,
        .oldLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
        .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        .image = swapChainInfo.images[swapChainImageIndex],
        .subresourceRange = {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .levelCount = 1, .layerCount = 1}
    };

    VkDependencyInfo postRenderDepInfo{.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
                                       .imageMemoryBarrierCount = 1,
                                       .pImageMemoryBarriers = &presentImageLayoutSwapBarrier};

    vkCmdPipelineBarrier2(commandBuffer, &postRenderDepInfo);

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
}

void drawFrame(VulkanCoreInfo& vulkanCoreInfo,
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
               UIManager& uIManager)
{
    vkWaitForFences(vulkanCoreInfo.device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

    uint32_t swapChainImageIndex;
    VkResult result = vkAcquireNextImageKHR(vulkanCoreInfo.device,
                                            swapChainInfo.swapChain,
                                            UINT64_MAX,
                                            imageAvailableSemaphores[currentFrame],
                                            VK_NULL_HANDLE,
                                            &swapChainImageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        recreateSwapChain(vulkanCoreInfo, swapChainInfo, commandPool);
        return;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }
    updateUniformBuffer(currentFrame, uniformBufferInfos, cameraHandler, swapChainInfo.extent);

    vkResetFences(vulkanCoreInfo.device, 1, &inFlightFences[currentFrame]);

    vkResetCommandBuffer(commandBuffers[currentFrame], /*VkCommandBufferResetFlagBits*/ 0);
    recordCommandBuffer(vulkanCoreInfo,
                        swapChainInfo,
                        graphicsPipelineInfo3d,
                        graphicsPipelineInfoSunShadow,
                        graphicsPipelineInfo2d,
                        descriptorSets3d[currentFrame],
                        descriptorSets2d[currentFrame],
                        commandBuffers[currentFrame],
                        swapChainImageIndex,
                        currentFrame,
                        vertexBufferManager,
                        cameraHandler);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers[currentFrame];

    VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    // std::cout << "here1" << std::endl;
    if (vkQueueSubmit(vulkanCoreInfo.graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }
    // std::cout << "here2" << std::endl;

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {swapChainInfo.swapChain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = &swapChainImageIndex;

    result = vkQueuePresentKHR(vulkanCoreInfo.presentQueue, &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
        recreateSwapChain(vulkanCoreInfo, swapChainInfo, commandPool);

        framebufferResized = false;
    }
    else if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
    }

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}
