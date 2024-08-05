#include "InventoryLayouts.hpp"

std::map<InventoryLayout, std::vector<InventorySlotLocation>> inventoryLayouts;

std::map<InventoryLayout, uint32_t> inventoryLayoutSizes = {
    {InventoryLayout::grid10x4Inventory, 40}
};

uint32_t getInventoryLayoutSize(InventoryLayout inventoryLayout)
{
    return inventoryLayoutSizes.at(inventoryLayout);
}

void generategrid10x4InventoryLayout(std::vector<InventorySlotLocation>& inventorySlotPositions)
{
    glm::vec2 slotSize = { 0.08f, 0.08f };
    glm::vec2 slotGap { 0.02f, 0.02f };

    for (int i = 0; i < 40; i++) {
        int x = i % 10;
        int y = i / 10;

        glm::vec2 location = {
            x * (slotSize.x + slotGap.x),
            y * (slotSize.y + slotGap.y)
        };

        InventorySlotLocation inventorySlotPosition(location, slotSize);
        inventorySlotPositions.push_back(inventorySlotPosition);
    }
}

void generateInventoryLayouts()
{
    std::vector<InventorySlotLocation> grid10x4InventorySlotPositions;
    inventoryLayouts.insert(std::make_pair(InventoryLayout::grid10x4Inventory, grid10x4InventorySlotPositions));
    generategrid10x4InventoryLayout(inventoryLayouts.at(InventoryLayout::grid10x4Inventory));
}

std::vector<InventorySlotLocation>& getInventoryLayoutPositions(InventoryLayout inventoryLayout)
{
    return inventoryLayouts.at(inventoryLayout);   
}
