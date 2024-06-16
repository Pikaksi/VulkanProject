#include "DebugMenu.hpp"

#include <iostream>

void DebugMenu::update(UIManager& uIManager, WorldManager& worldManager, CameraHandler& cameraHandler, GPUMemoryBlock& worldGPUMemoryBlock)
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

        updateUI(uIManager, lastRecordedFps, worldManager, cameraHandler, worldGPUMemoryBlock);
    }
}

void DebugMenu::updateUI(UIManager& uIManager, int fps, WorldManager& worldManager, CameraHandler& cameraHandler, GPUMemoryBlock& worldGPUMemoryBlock)
{
    leftUpCornerText->setText(
        "Fps: " + std::to_string(fps) + '\n' +
        "vertex count: " + std::to_string(worldGPUMemoryBlock.getVertexCount()) + '\n' +
        "Chunks loaded: " + std::to_string(worldManager.chunks.size()) + '\n' +
        "x: " + std::to_string(cameraHandler.position.x) + " y: " + std::to_string(cameraHandler.position.y) + " z: " + std::to_string(cameraHandler.position.z) + '\n' +
        "pitch: " + std::to_string(cameraHandler.rotationY) + " yaw: " + std::to_string(cameraHandler.rotationY)
    );

    uIManager.uIRefreshNeeded = true;
}

void DebugMenu::checkIfEnabledStatus(UIManager& uIManager)
{
    if (PlayerInputHandler::getInstance().f3Pressed) {
        std::cout << "pressed f3\n";
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
    leftUpCornerText = uIManager.createUIText(-1, -1, textSize, "");

    isEnabled = true;
}

void DebugMenu::disableMenu(UIManager& uIManager)
{
    uIManager.deleteUIText(leftUpCornerText);

    isEnabled = false;
}