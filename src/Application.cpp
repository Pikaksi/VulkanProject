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
#include "VulkanRendering/DeviceCreator.hpp"
#include "VulkanRendering/SwapChain.hpp"
#include "Player/PlayerControls.hpp"

#include <time.h>

void DestroyDebugUtilsMessengerEXT(VkInstance instance,
                                   VkDebugUtilsMessengerEXT debugMessenger,
                                   const VkAllocationCallbacks* pAllocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

void Application::run()
{
    initVulkan();

    initGame();

    mainLoop();

    cleanup();
}

void Application::framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
    auto app = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
    app->framebufferResized = true;
    app->uiManager.changeExtent({(uint32_t)width, (uint32_t)height});
}

void Application::initGame()
{
    PlayerInputHandler::getInstance().window = vulkanCoreInfo.window;
    PlayerInputHandler::getInstance().initGLFWControlCallbacks();

    uiManager.init(vulkanCoreInfo, swapChainInfo.extent);

    generateInventoryLayouts();

    debugMenu = DebugMenu();
    playerInventoryManager = PlayerInventoryManager();

    int worldMaxVertexCount = 100000000;
    int uiMaxVertexCount = 50000;
    vertexBufferManager = VertexBufferManager(vulkanCoreInfo, commandPool, worldMaxVertexCount, uiMaxVertexCount);

    generateBlockTextureLayerLookupTable();
}

void Application::initVulkan()
{
    createDevice(vulkanCoreInfo);
    glfwSetWindowUserPointer(vulkanCoreInfo.window, this);
    glfwSetFramebufferSizeCallback(vulkanCoreInfo.window, framebufferResizeCallback);

    VkPhysicalDeviceProperties physicalDeviceProperties;
    vkGetPhysicalDeviceProperties(vulkanCoreInfo.physicalDevice, &physicalDeviceProperties);
    maxVertexInputBindings = physicalDeviceProperties.limits.maxVertexInputBindings;

    commandPool = createCommandPool(vulkanCoreInfo);

    createSwapChain(vulkanCoreInfo, swapChainInfo, commandPool);

    descriptorSetLayout3d = createDescriptorSetLayout3d(vulkanCoreInfo);
    createGraphicsPipeline3d(vulkanCoreInfo, swapChainInfo, graphicsPipelineInfo3d, descriptorSetLayout3d);

    descriptorSetLayoutLod = createDescriptorSetLayoutLod(vulkanCoreInfo);
    createGraphicsPipelineLod(vulkanCoreInfo, swapChainInfo, graphicsPipelineInfoLod, descriptorSetLayoutLod);

    createGraphicsPipelineSunShadow(
        vulkanCoreInfo, swapChainInfo, graphicsPipelineInfoSunShadow, descriptorSetLayout3d);

    descriptorSetLayout2d = createDescriptorSetLayout2d(vulkanCoreInfo);
    createGraphicsPipeline2d(vulkanCoreInfo, swapChainInfo, graphicsPipelineInfo2d, descriptorSetLayout2d);

    createCameraUniformBuffers(vulkanCoreInfo, cameraUniformBuffers);

    blockTextureArraySampler = createBlockTextureSampler(vulkanCoreInfo);
    createBlockTextureArray(vulkanCoreInfo, blockTextureImageArray, commandPool, false);

    sunShadowSampler = createSunShadowSampler(vulkanCoreInfo);

    uiTextureSampler = createUITextureSampler(vulkanCoreInfo);
    createUIImageInfos(vulkanCoreInfo, commandPool, uiImageInfos);

    descriptorPool = createDescriptorPool(vulkanCoreInfo, uiImageInfos.size());

    descriptorSetsLod = createDescriptorSetsLod(vulkanCoreInfo,
                                              descriptorPool,
                                              descriptorSetLayoutLod,
                                              cameraUniformBuffers,
                                              swapChainInfo.sunShadowImage,
                                              sunShadowSampler);

    descriptorSets3d = createDescriptorSets3d(vulkanCoreInfo,
                                              descriptorPool,
                                              descriptorSetLayout3d,
                                              cameraUniformBuffers,
                                              blockTextureImageArray,
                                              blockTextureArraySampler,
                                              swapChainInfo.sunShadowImage,
                                              sunShadowSampler);

    descriptorSets2d =
        createDescriptorSets2d(vulkanCoreInfo, descriptorPool, descriptorSetLayout2d, uiImageInfos, uiTextureSampler);

    commandBuffers = createCommandBuffers(vulkanCoreInfo, commandPool);
    createSyncObjects(
        vulkanCoreInfo, swapChainInfo, imageAvailableSemaphores, renderFinishedSemaphores, inFlightFences);
}

void Application::mainLoop()
{
    std::cout << "\nEntering main loop\n" << std::endl; 
    while (!glfwWindowShouldClose(vulkanCoreInfo.window)) {

        glfwPollEvents();
        PlayerInputHandler::getInstance().update();

        cameraHandler.updateCameraTransform();

        gameMainLoop();

        FrameDrawInfo frame
        {
            .pipelineLod = graphicsPipelineInfoLod,
            .pipeline3d = graphicsPipelineInfo3d,
            .pipelineSunShadow = graphicsPipelineInfoSunShadow,
            .pipeline2d = graphicsPipelineInfo2d,
            .descriptorSetsLod = descriptorSetsLod,
            .descriptorSets3d = descriptorSets3d,
            .descriptorSets2d = descriptorSets2d,
            .uniformBufferInfos = cameraUniformBuffers,
            .currentFrame = currentFrame,
            .framebufferResized = framebufferResized,
            .commandBuffers = commandBuffers,
            .imageAvailableSemaphores = imageAvailableSemaphores,
            .renderFinishedSemaphores = renderFinishedSemaphores,
            .inFlightFences = inFlightFences,
            .commandPool = commandPool,
            .cameraHandler = cameraHandler,
            .vertexBufferManager = vertexBufferManager,
            .uiManager = uiManager,
            .debugMenu = debugMenu
        };

        drawFrame(vulkanCoreInfo, swapChainInfo, frame);
    }

    vkDeviceWaitIdle(vulkanCoreInfo.device);
}

void Application::gameMainLoop()
{
    glm::i32vec3 chunkLocation = glm::i32vec3(std::floor(cameraHandler.position.x / (float)CHUNK_SIZE),
                                              std::floor(cameraHandler.position.y / (float)CHUNK_SIZE),
                                              std::floor(cameraHandler.position.z / (float)CHUNK_SIZE));

    chunkRenderer.update(vulkanCoreInfo, commandPool, worldManager, vertexBufferManager, chunkLocation);

    playerInventoryManager.update(uiManager);

    updatePlayerControls(cameraHandler.position, worldManager, chunkRenderer, playerInventoryManager);

    debugMenu.update(uiManager, vertexBufferManager, worldManager, cameraHandler);
}

void Application::cleanup()
{
    vertexBufferManager.cleanUp(vulkanCoreInfo);

    uiManager.cleanup(vulkanCoreInfo);

    cleanupSwapChain(vulkanCoreInfo, swapChainInfo);

    vkDestroyPipeline(vulkanCoreInfo.device, graphicsPipelineInfo3d.pipeline, nullptr);
    vkDestroyPipelineLayout(vulkanCoreInfo.device, graphicsPipelineInfo3d.layout, nullptr);
    vkDestroyPipeline(vulkanCoreInfo.device, graphicsPipelineInfo2d.pipeline, nullptr);
    vkDestroyPipelineLayout(vulkanCoreInfo.device, graphicsPipelineInfo2d.layout, nullptr);

    vkDestroyPipeline(vulkanCoreInfo.device, graphicsPipelineInfoLod.pipeline, nullptr);
    vkDestroyPipelineLayout(vulkanCoreInfo.device, graphicsPipelineInfoLod.layout, nullptr);

    vkDestroyPipeline(vulkanCoreInfo.device, graphicsPipelineInfoSunShadow.pipeline, nullptr);
    vkDestroyPipelineLayout(vulkanCoreInfo.device, graphicsPipelineInfoSunShadow.layout, nullptr);
    vkDestroySampler(vulkanCoreInfo.device, sunShadowSampler, nullptr);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroyBuffer(vulkanCoreInfo.device, cameraUniformBuffers[i].buffer, nullptr);
        vkFreeMemory(vulkanCoreInfo.device, cameraUniformBuffers[i].memory, nullptr);
    }

    vkDestroyDescriptorPool(vulkanCoreInfo.device, descriptorPool, nullptr);

    vkDestroyImageView(vulkanCoreInfo.device, blockTextureImageArray.view, nullptr);
    vkDestroyImage(vulkanCoreInfo.device, blockTextureImageArray.image, nullptr);
    vkFreeMemory(vulkanCoreInfo.device, blockTextureImageArray.memory, nullptr);
    vkDestroySampler(vulkanCoreInfo.device, blockTextureArraySampler, nullptr);

    for (size_t i = 0; i < uiImageInfos.size(); i++) {
        vkDestroyImageView(vulkanCoreInfo.device, uiImageInfos[i].view, nullptr);
        vkDestroyImage(vulkanCoreInfo.device, uiImageInfos[i].image, nullptr);
        vkFreeMemory(vulkanCoreInfo.device, uiImageInfos[i].memory, nullptr);
    }
    vkDestroySampler(vulkanCoreInfo.device, uiTextureSampler, nullptr);

    vkDestroyDescriptorSetLayout(vulkanCoreInfo.device, descriptorSetLayoutLod, nullptr);
    vkDestroyDescriptorSetLayout(vulkanCoreInfo.device, descriptorSetLayout3d, nullptr);
    vkDestroyDescriptorSetLayout(vulkanCoreInfo.device, descriptorSetLayout2d, nullptr);

    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(vulkanCoreInfo.device, imageAvailableSemaphores[i], nullptr);
        vkDestroyFence(vulkanCoreInfo.device, inFlightFences[i], nullptr);
    }
    for (size_t i = 0; i < swapChainInfo.images.size(); i++) {
        vkDestroySemaphore(vulkanCoreInfo.device, renderFinishedSemaphores[i], nullptr);
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
