#include "Application.hpp"

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
}

void Application::initVulkan() {
    createDevice(vulkanCoreInfo);

    createSwapChain(vulkanCoreInfo, swapChainInfo);

    //createTextureImage();
    //createTextureImageView();
    //createTextureSampler();

    createSyncObjects();
}

void Application::mainLoop()
{
    while (!glfwWindowShouldClose(window)) {
        cameraHandler.updateCameraTransform();

        glfwPollEvents();

        gameMainLoop();
        
        drawFrame();
    }

    vkDeviceWaitIdle(device);
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

    chunkRenderer.renderNewChunks(worldManager, vertexBufferManager);
}

void Application::cleanup() {
    cleanupSwapChain();

    vkDestroyPipeline(device, graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
    vkDestroyRenderPass(device, renderPass, nullptr);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroyBuffer(device, uniformBuffers[i], nullptr);
        vkFreeMemory(device, uniformBuffersMemory[i], nullptr);
    }

    vkDestroyDescriptorPool(device, descriptorPool, nullptr);

    vkDestroySampler(device, textureSampler, nullptr);
    vkDestroyImageView(device, textureImageView, nullptr);

    vkDestroyImage(device, textureImage, nullptr);
    vkFreeMemory(device, textureImageMemory, nullptr);

    vertexBufferManager.cleanUpBuffers();

    vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
        vkDestroyFence(device, inFlightFences[i], nullptr);
    }

    vkDestroyCommandPool(device, commandPool, nullptr);
    
    vkDestroyDevice(device, nullptr);

    if (enableValidationLayers) {
        DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
    }

    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyInstance(instance, nullptr);

    glfwDestroyWindow(window);

    glfwTerminate();
}

