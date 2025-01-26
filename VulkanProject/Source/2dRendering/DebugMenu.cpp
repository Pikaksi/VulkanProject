#include "DebugMenu.hpp"

#include <iostream>

#include "UIText.hpp"

void DebugMenu::update(UIManager& uiManager, VertexBufferManager& vertexBufferManager, WorldManager& worldManager, CameraHandler& cameraHandler)
{
    checkIfEnabledStatus(uiManager);
    if (!isEnabled) {
        return;
    }

    fpsCounter++;

    auto currentTime = std::chrono::high_resolution_clock::now();
    auto timeFromLastUIRefresh = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - lastUIRefreshTime).count();
    if (timeFromLastUIRefresh > refreshInterval) {
        lastUIRefreshTime = currentTime;

        lastRecordedFPS = fpsCounter / refreshInterval;
        fpsCounter = 0;

    }
    drawUI(uiManager, vertexBufferManager, lastRecordedFPS, worldManager, cameraHandler);
}

void DebugMenu::drawUI(UIManager& uiManager, VertexBufferManager& vertexBufferManager, int fps, WorldManager& worldManager, CameraHandler& cameraHandler)
{
    createUIText(
        uiManager,
        { -1.0f, -1.0f },
        0.05f,
        UICenteringMode::topLeft,
        UICenteringMode::topLeft,
        "Fps: " + std::to_string(fps) + '\n' +
        "vertex count: " + std::to_string(vertexBufferManager.worldGPUMemoryBlock.getDataCount()) + '\n' +
        "Chunks loaded: " + std::to_string(worldManager.chunks.size()) + '\n' +
        "x: " + std::to_string(cameraHandler.position.x) + " y: " + std::to_string(cameraHandler.position.y) + " z: " + std::to_string(cameraHandler.position.z) + '\n' +
        "pitch: " + std::to_string(cameraHandler.rotationY) + " yaw: " + std::to_string(cameraHandler.rotationX) + "\n"
        //+ "world GPUMemoryBlock is using " + std::to_string(vertexBufferManager.worldGPUMemoryBlock.getDataCount()) + " bytes of data"
    );
}

void DebugMenu::checkIfEnabledStatus(UIManager& uiManager)
{
    if (PlayerInputHandler::getInstance().f3Pressed) {
        if (isEnabled) {
            disableMenu(uiManager);
        }
        else {
            enableMenu(uiManager);
        }
    }
}

void DebugMenu::enableMenu(UIManager& uiManager)
{
    isEnabled = true;
}

void DebugMenu::disableMenu(UIManager& uiManager)
{
    isEnabled = false;
}