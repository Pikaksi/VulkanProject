#pragma once

#include <chrono>

#include "UIManager.hpp"
#include "World/WorldManager.hpp"
#include "PlayerInputHandler.hpp"
#include "CameraHandler.hpp"
#include "Rendering/GPUMemoryBlock.hpp"

class DebugMenu
{
private:
	bool isEnabled = false;
	float refreshInterval;
	int fpsCounter = 0;
	int lastRecordedFPS = 0;
	float textSize = 0.05f;
    std::chrono::time_point<std::chrono::high_resolution_clock> lastFpsCounterResetTime;
    std::chrono::time_point<std::chrono::high_resolution_clock> lastUIRefreshTime;

public:
	DebugMenu() {}
	DebugMenu(float refreshInterval) : refreshInterval(refreshInterval) {}

	void update(UIManager& uIManager, VertexBufferManager& vertexBufferManager, WorldManager& worldManager, CameraHandler& cameraHandler);

private:
	void enableMenu(UIManager& uIManager);
	void disableMenu(UIManager& uIManager);
	void checkIfEnabledStatus(UIManager& uIManager);
	void drawUI(UIManager& uiManager, VertexBufferManager& vertexBufferManager, int fps, WorldManager& worldManager, CameraHandler& cameraHandler);
};
