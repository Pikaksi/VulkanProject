#include "DebugMenu.hpp"

#include "GPUMemoryBlock.hpp"
#include "UICenteringMode.hpp"
#include "UIText.hpp"
#include "assertm.hpp"

#include <string>
#include <time.h>

DebugMenuGlobals debugMenuGlobals{};

void DebugMenu::update(UIManager& uiManager,
                       VertexBufferManager& vertexBufferManager,
                       WorldManager& worldManager,
                       CameraHandler& cameraHandler)
{
    checkIfEnabledStatus(uiManager);
    debugMenuGlobals.isEnabledCopy = isEnabled;
    if (!isEnabled) {
        return;
    }

    fpsCounter++;

    auto currentTime = std::chrono::high_resolution_clock::now();
    auto timeFromLastUIRefresh =
        std::chrono::duration<float, std::chrono::seconds::period>(currentTime - lastUIRefreshTime).count();
    auto timeFromLastFrame =
        std::chrono::duration<double, std::chrono::milliseconds::period>(currentTime - lastFrameTime).count();

    maxFrameTimeMs = std::max(maxFrameTimeMs, timeFromLastFrame);

    if (timeFromLastUIRefresh > refreshInterval) {
        lastUIRefreshTime = currentTime;

        lastRecordedFPS = fpsCounter / timeFromLastUIRefresh;
        maxFrameTimeMsDisplay = maxFrameTimeMs;
        maxFrameTimeMs = 0;

        averageFenceTimeWaited = debugMenuGlobals.fenceWaitTimeSum / (double)debugMenuGlobals.fencesTimesWaited;
        debugMenuGlobals.fenceWaitTimeSum = 0.0;
        debugMenuGlobals.fencesTimesWaited = 0;

        averageGpuFrameTime = debugMenuGlobals.gpuFrameTimeSum / (double)debugMenuGlobals.gpuFramesTimed;
        debugMenuGlobals.gpuFrameTimeSum = 0.0;
        debugMenuGlobals.gpuFramesTimed = 0;

        averagePresentQueueTimeWaited = debugMenuGlobals.presentQueueWaitTimeSum / (double)debugMenuGlobals.presentQueuesWaited;
        debugMenuGlobals.presentQueueWaitTimeSum = 0.0;
        debugMenuGlobals.presentQueuesWaited = 0;

        fpsCounter = 0;
    }
    drawUI(uiManager, vertexBufferManager, lastRecordedFPS, worldManager, cameraHandler);

    lastFrameTime = currentTime;
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
        
        "Fps: " + std::to_string(fps)+ " frame time: " + std::to_string(1000.0 / fps) + '\n' +
        "Gpu average frame time: " + std::to_string(averageGpuFrameTime) + '\n' +
        "Max frame time: " + std::to_string(maxFrameTimeMsDisplay) + '\n' +
        "Fence wait duration: " + std::to_string(averageFenceTimeWaited) + " ms" + '\n' +
        "Present queue wait duration: " + std::to_string(averagePresentQueueTimeWaited) + " ms" + '\n' +
        "Chunk mesh time avg: " + std::to_string(debugMenuGlobals.chunkMeshTimeTotal / (double)debugMenuGlobals.chunksMeshed) + " micro s" + '\n' +
        "vertex count: " + std::to_string(gpuMemoryBlockDataSize(*vertexBufferManager.worldGpuMemoryBlock) / sizeof(Vertex)) +
        '\n' + "Chunks loaded: " + std::to_string(worldManager.chunks.size()) +
        " percentage compressed : " +
        std::to_string((float)debugMenuGlobals.chunkCountCompressed / (float)debugMenuGlobals.chunkCount) +
        " block size total: " + std::to_string(debugMenuGlobals.blockSizeTotal / 1024 / 1024) + "mB\n" +
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
    if (inputHandler.keyPressed(GLFW_KEY_F3)) {
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
