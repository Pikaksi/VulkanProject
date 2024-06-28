#include "Application.hpp"

#include "Constants.hpp"
#include "VulkanRendering/FrameDrawer.hpp"
#include "VulkanRendering/GraphicsPipeline.hpp"
#include "VulkanRendering/Buffers.hpp"
#include "VulkanRendering/Commands.hpp"
#include "VulkanRendering/Descriptor.hpp"
#include "Rendering/TextureCreator.hpp"
#include "3dRendering/BlockTexCoordinateLookup.hpp"

#include <thread>

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

    debugMenu = DebugMenu(0.25f);

    vertexBufferManager = VertexBufferManager(vulkanCoreInfo, commandPool, sizeof(Vertex) * 100000);

    generateBlockTexLayerLookupTable();
}

void Application::initVulkan()
{
    createDevice(vulkanCoreInfo);
    glfwSetWindowUserPointer(vulkanCoreInfo->window, this);
    glfwSetFramebufferSizeCallback(vulkanCoreInfo->window, framebufferResizeCallback);

    VkPhysicalDeviceProperties physicalDeviceProperties;
    vkGetPhysicalDeviceProperties(vulkanCoreInfo->physicalDevice, &physicalDeviceProperties);
    maxVertexInputBindings = physicalDeviceProperties.limits.maxVertexInputBindings;

    createSwapChain(vulkanCoreInfo, swapChainInfo);

    descriptorSetLayout3d = createDescriptorSetLayout3d(vulkanCoreInfo);
    createGraphicsPipeline3d(vulkanCoreInfo, swapChainInfo, graphicsPipelineInfo3d, descriptorSetLayout3d);
    descriptorSetLayout2d = createDescriptorSetLayout2d(vulkanCoreInfo);
    createGraphicsPipeline2d(vulkanCoreInfo, swapChainInfo, graphicsPipelineInfo2d, descriptorSetLayout2d);

    commandPool = createCommandPool(vulkanCoreInfo);

    createCameraUniformBuffers(vulkanCoreInfo, cameraUniformBuffers);

    createBlockTextureArray(vulkanCoreInfo, blockTextureImageArray, commandPool, false);
    blockTextureArraySampler = createBlockTextureSampler(vulkanCoreInfo);
    createTextureImage(vulkanCoreInfo, textTextureImage, commandPool, false, "TextSpriteSheet.png");
    textTextureSampler = createTextTextureSampler(vulkanCoreInfo);

    descriptorPool3d = createDescriptorPool3d(vulkanCoreInfo);
    descriptorSets3d = createDescriptorSets3d(vulkanCoreInfo, descriptorPool3d, descriptorSetLayout3d, cameraUniformBuffers, blockTextureImageArray, blockTextureArraySampler);

    descriptorPool2d = createDescriptorPool2d(vulkanCoreInfo);
    descriptorSets2d = createDescriptorSets2d(vulkanCoreInfo, descriptorPool2d, descriptorSetLayout2d, textTextureImage, textTextureSampler);

    commandBuffers = createCommandBuffers(vulkanCoreInfo, commandPool);
    createSyncObjects(vulkanCoreInfo, imageAvailableSemaphores, renderFinishedSemaphores, inFlightFences);
}

void Application::mainLoop()
{
    //fpsTimer = std::chrono::high_resolution_clock::now();

    while (!glfwWindowShouldClose(vulkanCoreInfo->window)) {

        /*auto currentTime = std::chrono::high_resolution_clock::now();
        float timePassed = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - fpsTimer).count();
        fpsTimer = currentTime;
        
        std::cout << "time passed " << timePassed << "\n";
        if (timePassed < 1000.0f / (float)MAX_FPS) {
            std::this_thread::sleep_for(std::chrono::milliseconds((int)timePassed));
        }*/

        glfwPollEvents();
        PlayerInputHandler::getInstance().update();

        cameraHandler.updateCameraTransform();

        debugMenu.update(uIManager, worldManager, cameraHandler, vertexBufferManager.worldGPUMemoryBlock);

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
            maxVertexInputBindings,
            framebufferResized,
            commandBuffers,
            imageAvailableSemaphores,
            renderFinishedSemaphores,
            inFlightFences,
            commandPool,
            cameraHandler,
            vertexBufferManager,
            uIManager);
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
    chunkRenderer.update(vulkanCoreInfo, commandPool, worldManager, vertexBufferManager, chunkLocation);
}

void Application::cleanup()
{
    vertexBufferManager.cleanUp(vulkanCoreInfo);
    uIManager.cleanUpAll(vulkanCoreInfo);

    cleanupSwapChain(vulkanCoreInfo, swapChainInfo);

    vkDestroyPipeline(vulkanCoreInfo->device, graphicsPipelineInfo3d->pipeline, nullptr);
    vkDestroyPipelineLayout(vulkanCoreInfo->device, graphicsPipelineInfo3d->layout, nullptr);
    vkDestroyPipeline(vulkanCoreInfo->device, graphicsPipelineInfo2d->pipeline, nullptr);
    vkDestroyPipelineLayout(vulkanCoreInfo->device, graphicsPipelineInfo2d->layout, nullptr);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroyBuffer(vulkanCoreInfo->device, cameraUniformBuffers[i]->buffer, nullptr);
        vkFreeMemory(vulkanCoreInfo->device, cameraUniformBuffers[i]->memory, nullptr);
    }

    vkDestroyDescriptorPool(vulkanCoreInfo->device, descriptorPool3d, nullptr);
    vkDestroyDescriptorPool(vulkanCoreInfo->device, descriptorPool2d, nullptr);

    vkDestroyImageView(vulkanCoreInfo->device, blockTextureImageArray->view, nullptr);
    vkDestroyImage(vulkanCoreInfo->device, blockTextureImageArray->image, nullptr);
    vkFreeMemory(vulkanCoreInfo->device, blockTextureImageArray->memory, nullptr);
    vkDestroySampler(vulkanCoreInfo->device, blockTextureArraySampler, nullptr);

    /*vkDestroyImageView(vulkanCoreInfo->device, blockTextureImage->view, nullptr);
    vkDestroyImage(vulkanCoreInfo->device, blockTextureImage->image, nullptr);
    vkFreeMemory(vulkanCoreInfo->device, blockTextureImage->memory, nullptr);
    vkDestroySampler(vulkanCoreInfo->device, blockTextureSampler, nullptr);*/

    vkDestroyImageView(vulkanCoreInfo->device, textTextureImage->view, nullptr);
    vkDestroyImage(vulkanCoreInfo->device, textTextureImage->image, nullptr);
    vkFreeMemory(vulkanCoreInfo->device, textTextureImage->memory, nullptr);
    vkDestroySampler(vulkanCoreInfo->device, textTextureSampler, nullptr);

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

