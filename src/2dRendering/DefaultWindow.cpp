#include "DefaultWindow.hpp"
#include <algorithm>
#include <iostream>

#include "UITextureCreator.hpp"
#include "UIHelperFunctions.hpp"

void createDefaultWindow(
	UIManager& uiManager,
	glm::vec2 location,
	glm::vec2 size,
	float topBarHeight,
	std::string title,
	glm::vec4 topBarColor,
	glm::vec4 bodyColor)
{
    createUIQuad(
        uiManager,
        location,
        { size.x, topBarHeight },
        { 0.0f, 0.0f },
        { 1.0f, 1.0f },
        UITexLayer::white,
        topBarColor);

    createUIQuad(
        uiManager,
        { location.x, location.y + topBarHeight },
        { size.x, size.y - topBarHeight },
        { 0.0f, 0.0f },
        { 1.0f, 1.0f },
        UITexLayer::white,
        bodyColor);

    createUIText(
        uiManager,
        location + glm::vec2(topBarHeight * 0.1f, topBarHeight * 0.1f),   
        topBarHeight * 0.8f,
        UICenteringMode::topLeft,
        UICenteringMode::topLeft,
        "Inventory");
}
