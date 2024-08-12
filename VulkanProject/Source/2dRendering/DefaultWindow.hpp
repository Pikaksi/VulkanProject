#pragma once

#include "UIText.hpp"

void createDefaultWindow(
	UIManager& uiManager,
	glm::vec2 location,
	glm::vec2 size,
	float topBarHeight,
	std::string title,
	glm::vec4 topBarColor,
	glm::vec4 bodyColor);