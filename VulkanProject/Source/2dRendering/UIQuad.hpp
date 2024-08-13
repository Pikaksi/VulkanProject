#pragma once

#include "UIManager.hpp"
#include "UITexLayer.hpp"
#include "UICenteringMode.hpp"

void createUIQuad(
    UIManager& uiManager,
    glm::vec2 location,
    glm::vec2 size,
    glm::vec2 texBottomLeft,
    glm::vec2 texTopRight,
    UITexLayer texLayer,
    glm::vec4 color);