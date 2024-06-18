#include <stdexcept>
#include <vector>
#include <array>

#include "FrameDrawer.hpp"
#include "SwapChain.hpp"
#include "CameraHandler.hpp"

void createSyncObjects(
    VulkanCoreInfo* vulkanCoreInfo,
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
        if (vkCreateSemaphore(vulkanCoreInfo->device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(vulkanCoreInfo->device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(vulkanCoreInfo->device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create synchronization objects for a frame!");
        }
    }
}

void updateUniformBuffer(uint32_t currentFrame, std::vector<UniformBufferInfo*>& uniformBufferInfos, CameraHandler cameraHandler, VkExtent2D swapChainExtent) {
    CameraUniformBufferObject ubo = cameraHandler.getCameraMatrix(swapChainExtent);

    memcpy(uniformBufferInfos[currentFrame]->mappingPointer, &ubo, sizeof(ubo));
}

void recordCommandBuffer(
    SwapChainInfo* swapChainInfo,
    GraphicsPipelineInfo* graphicsPipelineInfo3d,
    GraphicsPipelineInfo* graphicsPipelineInfo2d,
    VkDescriptorSet descriptorSet3d,
    VkDescriptorSet descriptorSet2d,
    VkCommandBuffer commandBuffer, 
    uint32_t imageIndex,
    uint32_t& maxVertexInputBindings,
    VertexBufferManager& vertexBufferManager,
    UIManager& uIManager)
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = swapChainInfo->renderPass;
    renderPassInfo.framebuffer = swapChainInfo->framebuffers[imageIndex];
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = swapChainInfo->extent;

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = { {0.3f, 0.7f, 0.9f, 1.0f} };
    clearValues[1].depthStencil = { 1.0f, 0 };

    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)swapChainInfo->extent.width;
    viewport.height = (float)swapChainInfo->extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = swapChainInfo->extent;


    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipelineInfo3d->pipeline);

    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    // render world from GPUMemoryBlocks
    VkBuffer vertexBuffer;
    std::vector<VkDeviceSize> vertexOffsets;
    std::vector<uint32_t> batchIndexCounts;
    vertexBufferManager.worldGPUMemoryBlock.getVerticesToRender(
        vertexBuffer,
        vertexOffsets,
        batchIndexCounts);

    unsigned long long testIndexCount = 0;

    for (int i = 0; i < vertexOffsets.size(); i++) {

        VkBuffer vertexBuffers[] = { vertexBuffer };

        VkDeviceSize offsets[] = { vertexOffsets[i] };
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

        vkCmdBindIndexBuffer(commandBuffer, vertexBufferManager.quadStripIndexBuffer, 0, VK_INDEX_TYPE_UINT32);

        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipelineInfo3d->layout, 0, 1, &descriptorSet3d, 0, nullptr);

        vkCmdDrawIndexed(commandBuffer, batchIndexCounts[i], 1, 0, 0, 0);
        testIndexCount += batchIndexCounts[i];
    }

    //std::cout << "rendered " << (double)testIndexCount / (double)1000000 << " million indices\n";

    // render blocks
    /*for (int i = 0; i < vertexBufferManager.vertexBuffers.size(); i++) {

        VkBuffer vertexBuffers[] = { vertexBufferManager.vertexBuffers[i] };
        
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

        vkCmdBindIndexBuffer(commandBuffer, vertexBufferManager.indexBuffers[i], 0, VK_INDEX_TYPE_UINT32);

        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipelineInfo3d->layout, 0, 1, &descriptorSet3d, 0, nullptr);

        vkCmdDrawIndexed(commandBuffer, vertexBufferManager.indexCounts[i], 1, 0, 0, 0);
    }*/

    // render UI
    if (uIManager.hasElementsToRender[uIManager.updatedUIIndex]) {
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipelineInfo2d->pipeline);

        VkBuffer vertexBuffers[] = { uIManager.vertexBuffer[uIManager.updatedUIIndex] };

        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

        vkCmdBindIndexBuffer(commandBuffer, uIManager.indexBuffer[uIManager.updatedUIIndex], 0, VK_INDEX_TYPE_UINT32);

        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipelineInfo2d->layout, 0, 1, &descriptorSet2d, 0, nullptr);

        vkCmdDrawIndexed(commandBuffer, uIManager.indexCount[uIManager.updatedUIIndex], 1, 0, 0, 0);
    }

    vkCmdEndRenderPass(commandBuffer);

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
}


void drawFrame(
    VulkanCoreInfo* vulkanCoreInfo,
    SwapChainInfo* swapChainInfo,
    GraphicsPipelineInfo* graphicsPipelineInfo3d,
    GraphicsPipelineInfo* graphicsPipelineInfo2d,
    std::vector<VkDescriptorSet> descriptorSets3d,
    std::vector<VkDescriptorSet> descriptorSets2d,
    std::vector<UniformBufferInfo*> uniformBufferInfos,
    uint32_t& currentFrame,
    uint32_t& maxVertexInputBindings,
    bool& framebufferResized,
    std::vector<VkCommandBuffer> commandBuffers,
    std::vector<VkSemaphore> imageAvailableSemaphores,
    std::vector<VkSemaphore> renderFinishedSemaphores,
    std::vector<VkFence> inFlightFences,
    VkCommandPool commandPool,
    CameraHandler& cameraHandler,
    VertexBufferManager& vertexBufferManager,
    UIManager& uIManager)
{
    vkWaitForFences(vulkanCoreInfo->device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(vulkanCoreInfo->device, swapChainInfo->swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        recreateSwapChain(vulkanCoreInfo, swapChainInfo);
        return;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }
    updateUniformBuffer(currentFrame, uniformBufferInfos, cameraHandler, swapChainInfo->extent);

    uIManager.refreshUI(vulkanCoreInfo, commandPool, swapChainInfo->extent.width, swapChainInfo->extent.height);

    vkResetFences(vulkanCoreInfo->device, 1, &inFlightFences[currentFrame]);

    vkResetCommandBuffer(commandBuffers[currentFrame], /*VkCommandBufferResetFlagBits*/ 0);
    recordCommandBuffer(
        swapChainInfo,
        graphicsPipelineInfo3d,
        graphicsPipelineInfo2d,
        descriptorSets3d[currentFrame],
        descriptorSets2d[currentFrame],
        commandBuffers[currentFrame],
        imageIndex,
        maxVertexInputBindings,
        vertexBufferManager,
        uIManager);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers[currentFrame];

    VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(vulkanCoreInfo->graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = { swapChainInfo->swapChain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = &imageIndex;

    result = vkQueuePresentKHR(vulkanCoreInfo->presentQueue, &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
        recreateSwapChain(vulkanCoreInfo, swapChainInfo);

        if (framebufferResized) {
            uIManager.refreshUI(vulkanCoreInfo, commandPool, swapChainInfo->extent.width, swapChainInfo->extent.height);
        }

        framebufferResized = false;
    }
    else if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
    }

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}