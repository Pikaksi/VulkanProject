#include "UIManager.hpp"

#include <vector>

#include "Constants.hpp"
#include "GPUMemoryBlock.hpp"
#include "UIHelperFunctions.hpp"
#include "PlayerInputHandler.hpp"
#include "VulkanTypes.hpp"

glm::vec2 UIManager::getMousePositionScreenSpace()
{
    return glm::vec2((float)PlayerInputHandler::getInstance().mouseLocationX * 2.0f / (float)extent.width - 1.0f,
                     (float)PlayerInputHandler::getInstance().mouseLocationY * 2.0f / (float)extent.height - 1.0f);
}

void UIManager::changeExtent(VkExtent2D newExtent)
{
    extent = newExtent;
    scalar = getScalarFromExtent(newExtent);
}

void UIManager::writeToBufferAndClear(uint32_t currentFrame)
{
    gpuMemoryBlockFreeAll(gpuMemoryBlocks[currentFrame]);
    if (vertices.size() == 0) {
        return;
    }
    gpuMemoryBlockAddHostVisible(gpuMemoryBlocks[currentFrame], (void*)vertices.data(), sizeof(Vertex2D) * vertices.size());
    vertices.clear();
}

void UIManager::init(VulkanCoreInfo& vulkanCoreInfo, VkExtent2D extent)
{
    changeExtent(extent);

    for (size_t i = 0; i < gpuMemoryBlocks.size(); i++) {
        gpuMemoryBlockInit(
            vulkanCoreInfo, gpuMemoryBlocks[i], UI_GPU_BUFFER_SIZE, true);
    }
}

void UIManager::cleanup(VulkanCoreInfo& vulkanCoreInfo)
{
    for (auto memoryBlock : gpuMemoryBlocks) {
        gpuMemoryBlockDestroy(vulkanCoreInfo, memoryBlock);
    }
}
