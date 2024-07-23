#include "DebugMenu.hpp"

#include <iostream>

void DebugMenu::update(UIManager& uIManager, VertexBufferManager& vertexBufferManager, WorldManager& worldManager, CameraHandler& cameraHandler)
{
    checkIfEnabledStatus(uIManager);
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

        updateUI(uIManager, vertexBufferManager, lastRecordedFps, worldManager, cameraHandler);
    }
}

void DebugMenu::updateUI(UIManager& uiManager, VertexBufferManager& vertexBufferManager, int fps, WorldManager& worldManager, CameraHandler& cameraHandler)
{
    debugMenuText->setText(
        "Fps: " + std::to_string(fps) + '\n' +
        "vertex count: " + std::to_string(vertexBufferManager.worldGPUMemoryBlock.getDataCount()) + '\n' +
        "Chunks loaded: " + std::to_string(worldManager.chunks.size()) + '\n' +
        "x: " + std::to_string(cameraHandler.position.x) + " y: " + std::to_string(cameraHandler.position.y) + " z: " + std::to_string(cameraHandler.position.z) + '\n' +
        "pitch: " + std::to_string(cameraHandler.rotationY) + " yaw: " + std::to_string(cameraHandler.rotationX)
    );
    uiManager.updateUIText(debugMenuText);
}

void DebugMenu::checkIfEnabledStatus(UIManager& uIManager)
{
    if (PlayerInputHandler::getInstance().f3Pressed) {
        if (isEnabled) {
            disableMenu(uIManager);
        }
        else {
            enableMenu(uIManager);
        }
    }
}

void DebugMenu::enableMenu(UIManager& uIManager)
{
    debugMenuText = uIManager.createUIText({-1.0f, -1.0f}, 0.05, "", UICenteringMode::TopLeft);

    isEnabled = true;
}

void DebugMenu::disableMenu(UIManager& uIManager)
{
    uIManager.deleteUIText(debugMenuText);

    isEnabled = false;
}