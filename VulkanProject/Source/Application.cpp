#include "Application.hpp"
#include "Constants.hpp"
#include "VulkanRendering/FrameDrawer.hpp"
#include "VulkanRendering/GraphicsPipeline.hpp"
#include "VulkanRendering/Buffers.hpp"
#include "VulkanRendering/Commands.hpp"
#include "VulkanRendering/Descriptor.hpp"
#include "VulkanRendering/ImageCreator.hpp"
#include "TextureCreator.hpp"

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

void Application::run() {
    initVulkan();

    initGame();

    mainLoop();

    cleanup();
}

void Application::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
    auto app = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
    app->framebufferResized = true;
}

void Application::initGame()
{
    PlayerInputHandler::getInstance().window = vulkanCoreInfo->window;
    PlayerInputHandler::getInstance().initGLFWControlCallbacks();

    vertexBufferManager.createVertexBuffer(vulkanCoreInfo, commandPool, vertexBufferManager.testVertexbuffer, vertexBufferManager.testVertexBufferMemory, vertexBufferManager.testVertices);
    vertexBufferManager.createIndexBuffer(vulkanCoreInfo, commandPool, vertexBufferManager.testIndexBuffer, vertexBufferManager.testIndexBufferMemory, vertexBufferManager.testIndices);
}

void Application::initVulkan() {
    createDevice(vulkanCoreInfo);
    glfwSetWindowUserPointer(vulkanCoreInfo->window, this);
    glfwSetFramebufferSizeCallback(vulkanCoreInfo->window, framebufferResizeCallback);

    createSwapChain(vulkanCoreInfo, swapChainInfo);

    descriptorSetLayout3d = createDescriptorSetLayout3d(vulkanCoreInfo);
    createGraphicsPipeline3d(vulkanCoreInfo, swapChainInfo, graphicsPipelineInfo3d, descriptorSetLayout3d);
    descriptorSetLayout2d = createDescriptorSetLayout2d(vulkanCoreInfo);
    createGraphicsPipeline2d(vulkanCoreInfo, swapChainInfo, graphicsPipelineInfo2d, descriptorSetLayout2d);

    commandPool = createCommandPool(vulkanCoreInfo);

    createCameraUniformBuffers(vulkanCoreInfo, cameraUniformBuffers);

    createTextureImage(vulkanCoreInfo, textureImage, commandPool, false);
    textureSampler = createTextureSampler(vulkanCoreInfo);

    descriptorPool3d = createDescriptorPool3d(vulkanCoreInfo);
    descriptorSets3d = createDescriptorSets3d(vulkanCoreInfo, descriptorPool3d, descriptorSetLayout3d, cameraUniformBuffers, textureImage, textureSampler);
    descriptorPool2d = createDescriptorPool2d(vulkanCoreInfo);
    descriptorSets2d = createDescriptorSets2d(vulkanCoreInfo, descriptorPool2d, descriptorSetLayout2d, textureImage, textureSampler);

    commandBuffers = createCommandBuffers(vulkanCoreInfo, commandPool);
    createSyncObjects(vulkanCoreInfo, imageAvailableSemaphores, renderFinishedSemaphores, inFlightFences);
}

void Application::mainLoop()
{
    while (!glfwWindowShouldClose(vulkanCoreInfo->window)) {
        cameraHandler.updateCameraTransform();

        glfwPollEvents();

        gameMainLoop();
        
        drawFrame(
            vulkanCoreInfo,
            swapChainInfo,
            graphicsPipelineInfo3d,
            graphicsPipelineInfo2d,
            descriptorSets3d,
            descriptorSets2d,
            cameraUniformBuffers,
            currentFrame,
            framebufferResized,
            commandBuffers,
            imageAvailableSemaphores,
            renderFinishedSemaphores,
            inFlightFences,
            cameraHandler,
            vertexBufferManager);
    }

    vkDeviceWaitIdle(vulkanCoreInfo->device);
}

void Application::gameMainLoop()
{
    glm::i32vec3 chunkLocation = glm::i32vec3(
        std::floor(cameraHandler.position.x / (float)CHUNK_SIZE), 
        std::floor(cameraHandler.position.y / (float)CHUNK_SIZE),
        std::floor(cameraHandler.position.z / (float)CHUNK_SIZE)
    );
    //std::cout << "location = " << chunkLocation.x << " " << chunkLocation.y << " " << chunkLocation.z << "\n";
    //std::cout << "camera location = " << cameraHandler.position.x << " " << cameraHandler.position.y << " " << cameraHandler.position.z << "\n";
    chunkRenderer.addChunksToBeRendered(chunkLocation);

    chunkRenderer.renderNewChunks(vulkanCoreInfo, commandPool, worldManager, vertexBufferManager);
}

void Application::cleanup() {
    // VulkanCoreInfo
    // ImageInfo
    // SwapChainInfo
    // GraphicsPipelineInfo
    // UniformBufferInfo
    // CameraUniformBufferObject
    cleanupSwapChain(vulkanCoreInfo, swapChainInfo);

    vkDestroyPipeline(vulkanCoreInfo->device, graphicsPipelineInfo3d->pipeline, nullptr);
    vkDestroyPipelineLayout(vulkanCoreInfo->device, graphicsPipelineInfo3d->layout, nullptr);
    vkDestroyPipeline(vulkanCoreInfo->device, graphicsPipelineInfo2d->pipeline, nullptr);
    vkDestroyPipelineLayout(vulkanCoreInfo->device, graphicsPipelineInfo2d->layout, nullptr);
    vkDestroyRenderPass(vulkanCoreInfo->device, swapChainInfo->renderPass, nullptr);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroyBuffer(vulkanCoreInfo->device, cameraUniformBuffers[i]->buffer, nullptr);
        vkFreeMemory(vulkanCoreInfo->device, cameraUniformBuffers[i]->memory, nullptr);
    }

    vkDestroyDescriptorPool(vulkanCoreInfo->device, descriptorPool3d, nullptr);
    vkDestroyDescriptorPool(vulkanCoreInfo->device, descriptorPool2d, nullptr);

    vkDestroySampler(vulkanCoreInfo->device, textureSampler, nullptr);

    vkDestroyImageView(vulkanCoreInfo->device, textureImage->view, nullptr);
    vkDestroyImage(vulkanCoreInfo->device, textureImage->image, nullptr);
    vkFreeMemory(vulkanCoreInfo->device, textureImage->memory, nullptr);

    vertexBufferManager.cleanUpBuffers(vulkanCoreInfo);

    vkDestroyDescriptorSetLayout(vulkanCoreInfo->device, descriptorSetLayout3d, nullptr);
    vkDestroyDescriptorSetLayout(vulkanCoreInfo->device, descriptorSetLayout2d, nullptr);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(vulkanCoreInfo->device, renderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(vulkanCoreInfo->device, imageAvailableSemaphores[i], nullptr);
        vkDestroyFence(vulkanCoreInfo->device, inFlightFences[i], nullptr);
    }

    vkDestroyCommandPool(vulkanCoreInfo->device, commandPool, nullptr);
    
    vkDestroyDevice(vulkanCoreInfo->device, nullptr);

    if (enableValidationLayers) {
        DestroyDebugUtilsMessengerEXT(vulkanCoreInfo->instance, vulkanCoreInfo->debugMessenger, nullptr);
    }

    vkDestroySurfaceKHR(vulkanCoreInfo->instance, vulkanCoreInfo->surface, nullptr);
    vkDestroyInstance(vulkanCoreInfo->instance, nullptr);

    glfwDestroyWindow(vulkanCoreInfo->window);

    glfwTerminate();
}

