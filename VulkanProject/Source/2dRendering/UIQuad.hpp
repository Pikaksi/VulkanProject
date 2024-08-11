#pragma once

#include "UIManager.hpp"
#include "UITexLayer.hpp"
#include "UICenteringMode.hpp"

void createUIQuad(UIManager& uiManager, glm::vec2 location, glm::vec2 size, glm::vec2 texTopLeft, glm::vec2 texBottomRight, UITexLayer texLayer, glm::vec4 color);