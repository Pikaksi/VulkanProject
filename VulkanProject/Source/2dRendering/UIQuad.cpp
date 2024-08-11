#include "UIQuad.hpp"

void createUIQuad(UIManager& uiManager, glm::vec2 location, glm::vec2 size, glm::vec2 texTopLeft, glm::vec2 texBottomRight, UITexLayer texLayer, glm::vec4 color)
{
    vertices.push_back(Vertex2D{ {location.x,              location.y,            }, color, texDownLeft,                     texLayer });
    vertices.push_back(Vertex2D{ {location.x,              location.y + size.y}, color, { texDownLeft.x, texUpRight.y }, texLayer });
    vertices.push_back(Vertex2D{ {location.x + size.x, location.y + size.y}, color, texUpRight,                      texLayer });
    vertices.push_back(Vertex2D{ {location.x + size.x, location.y,            }, color, { texUpRight.x, texDownLeft.y }, texLayer });
}
