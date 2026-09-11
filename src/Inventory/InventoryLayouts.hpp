#pragma once

#include <glm/vec2.hpp>

#include <map>
#include <vector>

enum class InventoryLayout
{
    grid10x4Inventory = 0,
    output1Input1 = 1,
    slot1 = 2,
};

struct InventorySlotLocation
{
    glm::vec2 location; 
    glm::vec2 size; 
};

std::vector<InventorySlotLocation>& getInventoryLayoutPositions(InventoryLayout InventoryLayout);
void generateInventoryLayouts();
