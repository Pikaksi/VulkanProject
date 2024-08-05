#pragma once

#include <glm/vec2.hpp>

#include <map>
#include <vector>

enum class InventoryLayout
{
    grid10x4Inventory = 0
};

struct InventorySlotLocation
{
    glm::vec2 location; 
    glm::vec2 size; 
};

std::vector<InventorySlotLocation>& getInventoryLayoutPositions(InventoryLayout InventoryLayout);
void generateInventoryLayouts();
uint32_t getInventoryLayoutSize(InventoryLayout inventoryLayout);
