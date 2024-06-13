#pragma once

#include <chrono>

#include "UIManager.hpp"
#include "World/WorldManager.hpp"
#include "PlayerInputHandler.hpp"
#include "CameraHandler.hpp"
#include "Rendering/GPUMemoryBlock.hpp"

#include "3dRendering/BlockTexCoordinateLookup.hpp"

class DebugMenu
{
private:
	bool isEnabled = false;
	float refreshInterval;
	int fpsCounter = 0;
	float textSize = 0.05f;
	UIText* leftUpCornerText;
	std::chrono::steady_clock::time_point lastFpsCounterResetTime;
	std::chrono::steady_clock::time_point lastUIRefreshTime;

public:
	DebugMenu() {}
	DebugMenu(float refreshInterval)
	{
		this->refreshInterval = refreshInterval;
	}

	void update(UIManager& uIManager, WorldManager& worldManager, CameraHandler& cameraHandler, GPUMemoryBlock& worldGPUMemoryBlock);

private:
	void enableMenu(UIManager& uIManager);
	void disableMenu(UIManager& uIManager);
	void checkIfEnabledStatus(UIManager& uIManager);
	void updateUI(UIManager& uIManager, int fps, WorldManager& worldManager, CameraHandler& cameraHandler, GPUMemoryBlock& worldGPUMemoryBlock);
};