#include "DebugMenu.hpp"

#include "GPUMemoryBlock.hpp"
#include "UICenteringMode.hpp"
#include "UIText.hpp"

#include <string>
#include <time.h>

DebugMenuGlobals debugMenuGlobals{};

void DebugMenu::update(UIManager& uiManager,
                       VertexBufferManager& vertexBufferManager,
                       WorldManager& worldManager,
                       CameraHandler& cameraHandler)
{
    checkIfEnabledStatus(uiManager);
    if (!isEnabled) {
        return;
    }

    fpsCounter++;

    clock_gettime(CLOCK_MONOTONIC, &timeOsFpsEnd);
    auto timeFromLastUIRefreshOs =
        timeOsFpsEnd.tv_sec + 1e-9 * timeOsFpsEnd.tv_nsec - (timeOsFpsStart.tv_sec + 1e-9 * timeOsFpsStart.tv_nsec);

    auto currentTime = std::chrono::high_resolution_clock::now();
    auto timeFromLastUIRefresh =
        std::chrono::duration<float, std::chrono::seconds::period>(currentTime - lastUIRefreshTime).count();
    if (timeFromLastUIRefresh > refreshInterval) {
        clock_gettime(CLOCK_MONOTONIC, &timeOsFpsStart);
        lastUIRefreshTime = currentTime;

        lastRecordedFPS = fpsCounter / timeFromLastUIRefresh;
        lastRecordedFPSOs = fpsCounter / timeFromLastUIRefreshOs;

        fpsCounter = 0;
    }
    drawUI(uiManager, vertexBufferManager, lastRecordedFPS, worldManager, cameraHandler);
}

void DebugMenu::drawUI(UIManager& uiManager,
                       VertexBufferManager& vertexBufferManager,
                       int fps,
                       WorldManager& worldManager,
                       CameraHandler& cameraHandler)
{
    // clang-format off
    createUIText(
        uiManager, {-1.0f, -1.0f}, 0.05f, UICenteringMode::topLeft, UICenteringMode::topLeft,
        
        "Fps: " + std::to_string(fps) + '\n' +
        "Fps Os clock: " + std::to_string(lastRecordedFPSOs) + '\n' +
        "Fence wait duration: " + std::to_string(fenceWaitTimeLast) + " ms" + '\n' +
        "Chunk mesh time avg: " + std::to_string(debugMenuGlobals.chunkGenTimeTotal / (double)debugMenuGlobals.chunksGenerated) + " micro s" + '\n' +
        "vertex count: " + std::to_string(gpuMemoryBlockDataCount(*vertexBufferManager.worldGpuMemoryBlock)) +
        '\n' + "Chunks loaded: " + std::to_string(worldManager.chunks.size()) + '\n' +
        "x: " + std::to_string(cameraHandler.position.x) + " y: " + std::to_string(cameraHandler.position.y) +
        " z: " + std::to_string(cameraHandler.position.z) + '\n' + "pitch: " +
        std::to_string(cameraHandler.rotationY) + " yaw: " + std::to_string(cameraHandler.rotationX) + "\n"
        //+ "world GPUMemoryBlock is using " + std::to_string(vertexBufferManager.worldGPUMemoryBlock.getDataCount()) +
        //" bytes of data"
    );

    // clang-format on
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

void DebugMenu::enableMenu(UIManager& uiManager) { isEnabled = true; }

void DebugMenu::disableMenu(UIManager& uiManager) { isEnabled = false; }
