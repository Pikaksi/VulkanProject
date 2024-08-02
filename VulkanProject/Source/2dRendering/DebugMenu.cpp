#include "DebugMenu.hpp"

#include <iostream>

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

        int lastRecordedFps = fpsCounter / refreshInterval;
        fpsCounter = 0;

        updateUI(uiManager, vertexBufferManager, lastRecordedFps, worldManager, cameraHandler);
    }
}

void DebugMenu::updateUI(UIManager& uiManager, VertexBufferManager& vertexBufferManager, int fps, WorldManager& worldManager, CameraHandler& cameraHandler)
{
    debugMenuText->text =
        "Fps: " + std::to_string(fps) + '\n' +
        "vertex count: " + std::to_string(vertexBufferManager.worldGPUMemoryBlock.getDataCount()) + '\n' +
        "Chunks loaded: " + std::to_string(worldManager.chunks.size()) + '\n' +
        "x: " + std::to_string(cameraHandler.position.x) + " y: " + std::to_string(cameraHandler.position.y) + " z: " + std::to_string(cameraHandler.position.z) + '\n' +
        "pitch: " + std::to_string(cameraHandler.rotationY) + " yaw: " + std::to_string(cameraHandler.rotationX);
    uiManager.updateUIObject(debugMenuText);
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
    debugMenuText = uiManager.createUIText({-1.0f, -1.0f}, 0.05f, "", UICenteringMode::topLeft);
    

    isEnabled = true;
}

void DebugMenu::disableMenu(UIManager& uiManager)
{
    uiManager.destroyUIObject(debugMenuText);

    isEnabled = false;
}