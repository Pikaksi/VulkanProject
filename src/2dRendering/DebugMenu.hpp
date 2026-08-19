#pragma once

#include <chrono>

#include "UIManager.hpp"
#include "World/WorldManager.hpp"
#include "PlayerInputHandler.hpp"
#include "CameraHandler.hpp"
#include "Rendering/GPUMemoryBlock.hpp"

struct DebugMenuGlobals
{
    uint64_t chunksMeshed = 0;
    double chunkMeshTimeTotal = 0.0;

    uint64_t chunkCount = 0;
    uint64_t chunkCountCompressed = 0;
    uint64_t blockSizeTotal = 0;
    bool isEnabledCopy = false;
};

extern DebugMenuGlobals debugMenuGlobals;

struct DebugMenu
{
    bool isEnabled = false;
    float refreshInterval = 0.2f;
    int fpsCounter = 0;
    int lastRecordedFPS = 0;
    float textSize = 0.05f;
    std::chrono::time_point<std::chrono::high_resolution_clock> lastFpsCounterResetTime;
    std::chrono::time_point<std::chrono::high_resolution_clock> lastUIRefreshTime;

    double fenceWaitTimeLast;

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
