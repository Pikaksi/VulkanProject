#include "Application.hpp"

#include "Constants.hpp"
#include "VulkanRendering/FrameDrawer.hpp"
#include "VulkanRendering/GraphicsPipeline.hpp"
#include "VulkanRendering/Buffers.hpp"
#include "VulkanRendering/Commands.hpp"
#include "VulkanRendering/Descriptor.hpp"
#include "Rendering/TextureCreator.hpp"
#include "3dRendering/BlockTexCoordinateLookup.hpp"
#include "2dRendering/UITextureCreator.hpp"
#include "Inventory/InventoryLayouts.hpp"

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
    app->uiManager.changeExtent({(uint32_t)width, (uint32_t)height});
}

void Application::initGame()
{
    PlayerInputHandler::getInstance().window = vulkanCoreInfo.window;
    PlayerInputHandler::getInstance().initGLFWControlCallbacks();

    uiManager.changeExtent(swapChainInfo.extent);

    generateInventoryLayouts();

    debugMenu = DebugMenu(0.25f);
    playerInventory = PlayerInventory();

    int worldMaxVertexCount = 500000;
    int uiMaxVertexCount = 50000;
    vertexBufferManager = VertexBufferManager(vulkanCoreInfo, commandPool, worldMaxVertexCount, uiMaxVertexCount);

    generateBlockTexLayerLookupTable();
}

void Application::initVulkan()
{
    createDevice(vulkanCoreInfo);
    glfwSetWindowUserPointer(vulkanCoreInfo.window, this);
    glfwSetFramebufferSizeCallback(vulkanCoreInfo.window, framebufferResizeCallback);

    VkPhysicalDeviceProperties physicalDeviceProperties;
    vkGetPhysicalDeviceProperties(vulkanCoreInfo.physicalDevice, &physicalDeviceProperties);
    maxVertexInputBindings = physicalDeviceProperties.limits.maxVertexInputBindings;

    createSwapChain(vulkanCoreInfo, swapChainInfo);

    descriptorSetLayout3d = createDescriptorSetLayout3d(vulkanCoreInfo);
    createGraphicsPipeline3d(vulkanCoreInfo, swapChainInfo, graphicsPipelineInfo3d, descriptorSetLayout3d);
    descriptorSetLayout2d = createDescriptorSetLayout2d(vulkanCoreInfo);
    createGraphicsPipeline2d(vulkanCoreInfo, swapChainInfo, graphicsPipelineInfo2d, descriptorSetLayout2d);

    commandPool = createCommandPool(vulkanCoreInfo);

    createCameraUniformBuffers(vulkanCoreInfo, cameraUniformBuffers);

    blockTextureArraySampler = createBlockTextureSampler(vulkanCoreInfo);
    createBlockTextureArray(vulkanCoreInfo, blockTextureImageArray, commandPool, false);

    uiTextureSampler = createUITextureSampler(vulkanCoreInfo);
    createUIImageInfos(vulkanCoreInfo, commandPool, uiImageInfos);

    descriptorPool3d = createDescriptorPool3d(vulkanCoreInfo);
    descriptorSets3d = createDescriptorSets3d(vulkanCoreInfo, descriptorPool3d, descriptorSetLayout3d, cameraUniformBuffers, blockTextureImageArray, blockTextureArraySampler);

    descriptorPool2d = createDescriptorPool2d(vulkanCoreInfo, uiImageInfos.size());
    descriptorSets2d = createDescriptorSets2d(vulkanCoreInfo, descriptorPool2d, descriptorSetLayout2d, uiImageInfos, uiTextureSampler);

    commandBuffers = createCommandBuffers(vulkanCoreInfo, commandPool);
    createSyncObjects(vulkanCoreInfo, imageAvailableSemaphores, renderFinishedSemaphores, inFlightFences);
}

void Application::mainLoop()
{
    while (!glfwWindowShouldClose(vulkanCoreInfo.window)) {

        glfwPollEvents();
        PlayerInputHandler::getInstance().update();

        cameraHandler.updateCameraTransform();

        gameMainLoop();

        uiManager.updateScreen(swapChainInfo.extent, vulkanCoreInfo, commandPool, vertexBufferManager);
        
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
            commandPool,
            cameraHandler,
            vertexBufferManager,
            uiManager);
    }

    vkDeviceWaitIdle(vulkanCoreInfo.device);
}

void Application::gameMainLoop()
{
    glm::i32vec3 chunkLocation = glm::i32vec3(
        std::floor(cameraHandler.position.x / (float)CHUNK_SIZE), 
        std::floor(cameraHandler.position.y / (float)CHUNK_SIZE),
        std::floor(cameraHandler.position.z / (float)CHUNK_SIZE)
    );
    chunkRenderer.update(vulkanCoreInfo, commandPool, worldManager, vertexBufferManager, chunkLocation);

    playerInventory.update(uiManager);

    debugMenu.update(uiManager, vertexBufferManager, worldManager, cameraHandler);
}

void Application::cleanup()
{
    vertexBufferManager.cleanUp(vulkanCoreInfo);

    cleanupSwapChain(vulkanCoreInfo, swapChainInfo);

    vkDestroyPipeline(vulkanCoreInfo.device, graphicsPipelineInfo3d.pipeline, nullptr);
    vkDestroyPipelineLayout(vulkanCoreInfo.device, graphicsPipelineInfo3d.layout, nullptr);
    vkDestroyPipeline(vulkanCoreInfo.device, graphicsPipelineInfo2d.pipeline, nullptr);
    vkDestroyPipelineLayout(vulkanCoreInfo.device, graphicsPipelineInfo2d.layout, nullptr);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroyBuffer(vulkanCoreInfo.device, cameraUniformBuffers[i].buffer, nullptr);
        vkFreeMemory(vulkanCoreInfo.device, cameraUniformBuffers[i].memory, nullptr);
    }

    vkDestroyDescriptorPool(vulkanCoreInfo.device, descriptorPool3d, nullptr);
    vkDestroyDescriptorPool(vulkanCoreInfo.device, descriptorPool2d, nullptr);

    vkDestroyImageView(vulkanCoreInfo.device, blockTextureImageArray.view, nullptr);
    vkDestroyImage(vulkanCoreInfo.device, blockTextureImageArray.image, nullptr);
    vkFreeMemory(vulkanCoreInfo.device, blockTextureImageArray.memory, nullptr);
    vkDestroySampler(vulkanCoreInfo.device, blockTextureArraySampler, nullptr);

    for (int i = 0; i < uiImageInfos.size(); i++) {
        vkDestroyImageView(vulkanCoreInfo.device, uiImageInfos[i].view, nullptr);
        vkDestroyImage(vulkanCoreInfo.device, uiImageInfos[i].image, nullptr);
        vkFreeMemory(vulkanCoreInfo.device, uiImageInfos[i].memory, nullptr);
    }
    vkDestroySampler(vulkanCoreInfo.device, uiTextureSampler, nullptr);

    vkDestroyDescriptorSetLayout(vulkanCoreInfo.device, descriptorSetLayout3d, nullptr);
    vkDestroyDescriptorSetLayout(vulkanCoreInfo.device, descriptorSetLayout2d, nullptr);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(vulkanCoreInfo.device, renderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(vulkanCoreInfo.device, imageAvailableSemaphores[i], nullptr);
        vkDestroyFence(vulkanCoreInfo.device, inFlightFences[i], nullptr);
    }

    vkDestroyCommandPool(vulkanCoreInfo.device, commandPool, nullptr);
    
    vkDestroyDevice(vulkanCoreInfo.device, nullptr);

    if (enableValidationLayers) {
        DestroyDebugUtilsMessengerEXT(vulkanCoreInfo.instance, vulkanCoreInfo.debugMessenger, nullptr);
    }

    vkDestroySurfaceKHR(vulkanCoreInfo.instance, vulkanCoreInfo.surface, nullptr);
    vkDestroyInstance(vulkanCoreInfo.instance, nullptr);

    glfwDestroyWindow(vulkanCoreInfo.window);

    glfwTerminate();
}

