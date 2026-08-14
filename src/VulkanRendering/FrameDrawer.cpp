#include <stdexcept>
#include <vector>
#include <array>

#include "FrameDrawer.hpp"
#include "Constants.hpp"
#include "DebugMenu.hpp"
#include "GPUMemoryBlock.hpp"
#include "SwapChain.hpp"
#include "CameraHandler.hpp"
#include "UIManager.hpp"
#include "VulkanTypes.hpp"
#include "World/Chunk.hpp"
#include "vulkan/vulkan_core.h"

void createDrawCallBuffers(VulkanCoreInfo& vulkanCoreInfo, std::vector<GpuMemoryBlock> drawCallBuffers)
{
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        drawCallBuffers.push_back(GpuMemoryBlock{});
        gpuMemoryBlockInit(vulkanCoreInfo, drawCallBuffers.back(), 0 /*1000000*/, true);
    }
}

void createSyncObjects(VulkanCoreInfo& vulkanCoreInfo,
                       SwapChainInfo& swapChainInfo,
                       std::vector<VkSemaphore>& imageAvailableSemaphores,
                       std::vector<VkSemaphore>& renderFinishedSemaphores,
                       std::vector<VkFence>& inFlightFences)
{
    imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores.resize(swapChainInfo.images.size());
    inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateFence(vulkanCoreInfo.device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create fences");
        }
    }
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateSemaphore(vulkanCoreInfo.device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) !=
            VK_SUCCESS) {
            throw std::runtime_error("Failed to create image available semaphores");
        }
    }
    for (size_t i = 0; i < swapChainInfo.images.size(); i++) {
        if (vkCreateSemaphore(vulkanCoreInfo.device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) !=
            VK_SUCCESS) {
            throw std::runtime_error("Failed to create render finished semaphores");
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
    const int viewingFrustumSafetyOffset = 0;

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

bool chunkIsInViewingFrustumLod(glm::vec3& cameraLocation,
                                glm::ivec3& chunkLocation,
                                ChunkCenterOffsets& chunkCenterOffsets,
                                ViewingFrustumNormals& viewingFrustumNormals,
                                int lod)
{
    int lodScaling = 1 << lod;
    return glm::dot(static_cast<glm::vec3>(chunkLocation * CHUNK_SIZE + chunkCenterOffsets.top * lodScaling) -
                        cameraLocation,
                    viewingFrustumNormals.top) < 0.0f &&
           glm::dot(static_cast<glm::vec3>(chunkLocation * CHUNK_SIZE + chunkCenterOffsets.bottom * lodScaling) -
                        cameraLocation,
                    viewingFrustumNormals.bottom) < 0.0f &&
           glm::dot(static_cast<glm::vec3>(chunkLocation * CHUNK_SIZE + chunkCenterOffsets.right * lodScaling) -
                        cameraLocation,
                    viewingFrustumNormals.right) < 0.0f &&
           glm::dot(static_cast<glm::vec3>(chunkLocation * CHUNK_SIZE + chunkCenterOffsets.left * lodScaling) -
                        cameraLocation,
                    viewingFrustumNormals.left) < 0.0f;
}

void recordCommandBuffer(SwapChainInfo& swapChainInfo, FrameDrawInfo& draw, uint32_t swapChainImageIndex)
{
    auto commandBuffer = draw.commandBuffers[draw.currentFrame];

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    VkBuffer worldVertexBuffer;
    std::vector<WorldDrawCallData> worldDrawCallData;
    VkBuffer worldIndexBuffer;
    draw.vertexBufferManager.getWorldGeometryForRendering(worldVertexBuffer, worldDrawCallData, worldIndexBuffer);
    /*std::cout << "chunk locations are: ";
    for (auto a : worldDrawCallData) std::cout << a.chunkLocation.x << " " << a.chunkLocation.y << " " <<
    a.chunkLocation.z << " | "; std::cout << std::endl;*/

    // ---------------- SUN SHADOW PASS ----------------

    {

        VkImageMemoryBarrier2 shadowImageToAttachmentBarrier{
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
                                               .pImageMemoryBarriers = &shadowImageToAttachmentBarrier};

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
        .offset = {0,             0             },
          .extent{.width = 2048, .height = 2048}
    };
    sunShadowRenderingInfo.layerCount = 1;

    vkCmdBeginRendering(commandBuffer, &sunShadowRenderingInfo);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, draw.pipelineSunShadow.pipeline);
    vkCmdSetDepthBias(commandBuffer, /*constant*/ 0.0f, /*clamp*/ 0.0f, /*slope*/ -1.0f);

    {
        vkCmdBindIndexBuffer(commandBuffer, worldIndexBuffer, 0, VK_INDEX_TYPE_UINT32);
        vkCmdBindDescriptorSets(commandBuffer,
                                VK_PIPELINE_BIND_POINT_GRAPHICS,
                                draw.pipeline3d.layout,
                                0,
                                1,
                                &draw.descriptorSets3d[draw.currentFrame],
                                0,
                                nullptr);
        VkBuffer vertexBuffers[] = {worldVertexBuffer};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

        for (size_t i = 0; i < worldDrawCallData.size(); i++) {
            WorldDrawCallData drawCallData = worldDrawCallData[i];
            if (!drawCallData.fullDetail)
                continue;

            PushConstant3d pushConstant = {drawCallData.chunkLocation * CHUNK_SIZE};
            vkCmdPushConstants(commandBuffer,
                               draw.pipeline3d.layout,
                               VK_SHADER_STAGE_VERTEX_BIT,
                               0,
                               sizeof(PushConstant3d),
                               &pushConstant);

            // get index count by multiplying vertex count by 1.5
            vkCmdDrawIndexed(commandBuffer,
                             drawCallData.dataSize / sizeof(Vertex) / 2 * 3,
                             1,
                             0,
                             drawCallData.memoryLocation / sizeof(Vertex),
                             0);
        }
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

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, draw.pipeline3d.pipeline);

    ViewingFrustumNormals viewingFrustumNormals;
    draw.cameraHandler.getViewingFrustumNormals(swapChainInfo.extent, viewingFrustumNormals);
    ChunkCenterOffsets chunkCenterOffsets;
    getChunkCenterOffsets(chunkCenterOffsets, viewingFrustumNormals);

    {
        vkCmdBindIndexBuffer(commandBuffer, worldIndexBuffer, 0, VK_INDEX_TYPE_UINT32);
        vkCmdBindDescriptorSets(commandBuffer,
                                VK_PIPELINE_BIND_POINT_GRAPHICS,
                                draw.pipeline3d.layout,
                                0,
                                1,
                                &draw.descriptorSets3d[draw.currentFrame],
                                0,
                                nullptr);
        VkBuffer vertexBuffers[] = {worldVertexBuffer};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

        for (int i = 0; i < worldDrawCallData.size(); i++) {
            WorldDrawCallData drawCallData = worldDrawCallData[i];
            if (!drawCallData.fullDetail)
                continue;

            if (!chunkIsInViewingFrustumLod(draw.cameraHandler.position,
                                            drawCallData.chunkLocation,
                                            chunkCenterOffsets,
                                            viewingFrustumNormals,
                                            0)) {
                continue;
            }

            // std::cout << "drawing with loc = " << drawCallData.memoryLocation << " size = " << drawCallData.dataSize
            // << std::endl;
            PushConstant3d pushConstant = {drawCallData.chunkLocation * CHUNK_SIZE};
            vkCmdPushConstants(commandBuffer,
                               draw.pipeline3d.layout,
                               VK_SHADER_STAGE_VERTEX_BIT,
                               0,
                               sizeof(PushConstant3d),
                               &pushConstant);
            // get index count by multiplying vertex count by 1.5
            vkCmdDrawIndexed(commandBuffer,
                             drawCallData.dataSize / sizeof(Vertex) / 2 * 3,
                             1,
                             0,
                             drawCallData.memoryLocation / sizeof(Vertex),
                             0);
        }
    }

    // ---------------- LOD PASS ----------------

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, draw.pipelineLod.pipeline);
    {
        vkCmdBindIndexBuffer(commandBuffer, worldIndexBuffer, 0, VK_INDEX_TYPE_UINT32);
        vkCmdBindDescriptorSets(commandBuffer,
                                VK_PIPELINE_BIND_POINT_GRAPHICS,
                                draw.pipelineLod.layout,
                                0,
                                1,
                                &draw.descriptorSetsLod[draw.currentFrame],
                                0,
                                nullptr);
        VkBuffer vertexBuffers[] = {worldVertexBuffer};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

        for (int i = 0; i < worldDrawCallData.size(); i++) {
            WorldDrawCallData drawCallData = worldDrawCallData[i];
            if (drawCallData.fullDetail)
                continue;

            if (!chunkIsInViewingFrustumLod(draw.cameraHandler.position,
                                            drawCallData.chunkLocation,
                                            chunkCenterOffsets,
                                            viewingFrustumNormals,
                                            drawCallData.lod)) {
                continue;
            }

            PushConstant3dLod pushConstant = {drawCallData.chunkLocation * CHUNK_SIZE, 32.0f * (1 << drawCallData.lod)};
            vkCmdPushConstants(commandBuffer,
                               draw.pipelineLod.layout,
                               VK_SHADER_STAGE_VERTEX_BIT,
                               0,
                               sizeof(PushConstant3dLod),
                               &pushConstant);
            // get index count by multiplying vertex count by 1.5
            vkCmdDrawIndexed(commandBuffer,
                             drawCallData.dataSize / sizeof(VertexLod) / 2 * 3,
                             1,
                             0,
                             drawCallData.memoryLocation / sizeof(VertexLod),
                             0);
        }
    }

    // ---------------- UI PASS----------------

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, draw.pipeline2d.pipeline);

    draw.uiManager.writeToBufferAndClear(draw.currentFrame);
    VkBuffer uiVertexBuffer;
    std::vector<VkDeviceSize> uiVertexOffsets;
    std::vector<uint64_t> uiBatchSizes;
    gpuMemoryBlockGetData(
        draw.uiManager.gpuMemoryBlocks[draw.currentFrame], uiVertexBuffer, uiVertexOffsets, uiBatchSizes);
    VkBuffer uiIndexBuffer = draw.vertexBufferManager.quadStripIndexBuffer.getBuffer();

    vkCmdBindIndexBuffer(commandBuffer, uiIndexBuffer, 0, VK_INDEX_TYPE_UINT32);
    vkCmdBindDescriptorSets(commandBuffer,
                            VK_PIPELINE_BIND_POINT_GRAPHICS,
                            draw.pipeline2d.layout,
                            0,
                            1,
                            &draw.descriptorSets2d[draw.currentFrame],
                            0,
                            nullptr);
    for (int i = 0; i < uiVertexOffsets.size(); i++) {
        VkBuffer vertexBuffers[] = {uiVertexBuffer};
        VkDeviceSize offsets[] = {uiVertexOffsets[i]};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

        vkCmdDrawIndexed(commandBuffer, uiBatchSizes[i] / sizeof(Vertex2D) / 2 * 3, 1, 0, 0, 0);
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

void drawFrame(VulkanCoreInfo& vulkanCoreInfo, SwapChainInfo& swapChainInfo, FrameDrawInfo& draw)
{
    auto debugStartWait = std::chrono::high_resolution_clock::now();

    vkWaitForFences(vulkanCoreInfo.device, 1, &draw.inFlightFences[draw.currentFrame], VK_TRUE, UINT64_MAX);

    auto debugEndWait = std::chrono::high_resolution_clock::now();
    draw.debugMenu.fenceWaitTimeLast =
        std::chrono::duration<float, std::chrono::milliseconds::period>(debugEndWait - debugStartWait).count();

    uint32_t swapChainImageIndex;
    VkResult result = vkAcquireNextImageKHR(vulkanCoreInfo.device,
                                            swapChainInfo.swapChain,
                                            UINT64_MAX,
                                            draw.imageAvailableSemaphores[draw.currentFrame],
                                            VK_NULL_HANDLE,
                                            &swapChainImageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        recreateSwapChain(vulkanCoreInfo, swapChainInfo, draw.commandPool);
        return;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }
    updateUniformBuffer(draw.currentFrame, draw.uniformBufferInfos, draw.cameraHandler, swapChainInfo.extent);

    vkResetFences(vulkanCoreInfo.device, 1, &draw.inFlightFences[draw.currentFrame]);

    vkResetCommandBuffer(draw.commandBuffers[draw.currentFrame], /*VkCommandBufferResetFlagBits*/ 0);
    recordCommandBuffer(swapChainInfo, draw, swapChainImageIndex);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {draw.imageAvailableSemaphores[draw.currentFrame]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &draw.commandBuffers[draw.currentFrame];

    VkSemaphore signalSemaphores[] = {draw.renderFinishedSemaphores[swapChainImageIndex]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(vulkanCoreInfo.graphicsQueue, 1, &submitInfo, draw.inFlightFences[draw.currentFrame]) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {swapChainInfo.swapChain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = &swapChainImageIndex;

    result = vkQueuePresentKHR(vulkanCoreInfo.presentQueue, &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || draw.framebufferResized) {
        recreateSwapChain(vulkanCoreInfo, swapChainInfo, draw.commandPool);

        draw.framebufferResized = false;
    }
    else if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
    }

    draw.currentFrame = (draw.currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}
