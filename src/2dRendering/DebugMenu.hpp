#pragma once

#include <chrono>

#include "UIManager.hpp"
#include "World/WorldManager.hpp"
#include "PlayerInputHandler.hpp"
#include "CameraHandler.hpp"
#include "Rendering/GPUMemoryBlock.hpp"

struct DebugMenu
{
    bool isEnabled = false;
    float refreshInterval = 0.2f;
    int fpsCounter = 0;
    int lastRecordedFPS = 0;
    int lastRecordedFPSOs = 0;
    float textSize = 0.05f;
    std::chrono::time_point<std::chrono::high_resolution_clock> lastFpsCounterResetTime;
    std::chrono::time_point<std::chrono::high_resolution_clock> lastUIRefreshTime;

    double fenceWaitTimeLast;
    double frameDrawTimeCpuLast;
    double frameDrawTimeRealLast;

    timespec timeOsFpsStart;
    timespec timeOsFpsEnd;

    DebugMenu() {}

    void update(UIManager& uIManager,
                VertexBufferManager& vertexBufferManager,
                WorldManager& worldManager,
                CameraHandler& cameraHandler);

    void enableMenu(UIManager& uIManager);
    void disableMenu(UIManager& uIManager);
    void checkIfEnabledStatus(UIManager& uIManager);
    void drawUI(UIManager& uiManager,
                VertexBufferManager& vertexBufferManager,
                int fps,
                WorldManager& worldManager,
                CameraHandler& cameraHandler);
};
