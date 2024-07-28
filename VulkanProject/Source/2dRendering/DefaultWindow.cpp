#include "DefaultWindow.hpp"

#include <algorithm>
#include <iostream>

#include "UITextureCreator.hpp"
#include "UIHelperFunctions.hpp"

void DefaultWindow::addMeshData(VkExtent2D extent, std::vector<Vertex2D>& vertices)
{
    glm::vec2 scalar = { 1, 1 };//getScalarFromExtent(extent);
    glm::vec2 realSize = size * scalar;
    float realTopBarHeight = topBarHeight * scalar.y;

    glm::vec2 realLocation = getCenteredLocation(location, size, centeringMode);
    realLocation *= scalar;

    addMeshDataScaledCentered(extent, vertices, realLocation, realSize, realTopBarHeight);
}

void DefaultWindow::addMeshDataScaledCentered(VkExtent2D extent, std::vector<Vertex2D>& vertices, glm::vec2 location, glm::vec2 size, float topBarHeight)
{
    UIQuad topBar(
        location,
        { size.x, topBarHeight },
        { 0.0f, 0.0f },
        { 1.0f, 1.0f },
        UITexLayer::white,
        topBarColor,
        UICenteringMode::topLeft,
        false);
    topBar.addMeshData(extent, vertices);

    UIQuad body(
        { location.x, location.y + topBarHeight },
        { size.x, size.y - topBarHeight },
        { 0.0f, 0.0f },
        { 1.0f, 1.0f },
        UITexLayer::white,
        bodyColor,
        UICenteringMode::topLeft,
        false);
    body.addMeshData(extent, vertices);

    UIText uiText(
        location + glm::vec2(topBarHeight * 0.1f, topBarHeight * 0.1f),
        topBarHeight * 0.8f,
        title,
        UICenteringMode::topLeft);
    uiText.addMeshData(extent, vertices);
}
