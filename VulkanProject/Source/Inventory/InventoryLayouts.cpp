#include "InventoryLayouts.hpp"

std::map<InventoryLayout, std::vector<InventorySlotLocation>> inventoryLayouts;

std::map<InventoryLayout, uint32_t> inventoryLayoutSizes = {
    {InventoryLayout::grid10x4Inventory, 40}
};

uint32_t getInventoryLayoutSize(InventoryLayout inventoryLayout)
{
    return inventoryLayoutSizes.at(inventoryLayout);
}

std::vector<InventorySlotLocation> generategrid10x4InventoryLayout()
{
    std::vector<InventorySlotLocation> inventorySlotLocations;
    glm::vec2 startingOffset = { 0.01f, 0.1f };
    glm::vec2 slotSize = { 0.08f, 0.08f };
    glm::vec2 slotGap { 0.02f, 0.02f };

    for (int i = 0; i < 40; i++) {
        int x = i % 10;
        int y = i / 10;

        glm::vec2 location = {
            x * (slotSize.x + slotGap.x),
            y * (slotSize.y + slotGap.y)
        };
        location += startingOffset;
        // scale from 0 - 1 to -1 - 1 range
        glm::vec2 screenLocation = location * glm::vec2(2, 2) - glm::vec2(1, 1);
        glm::vec2 screenSize = slotSize * glm::vec2(2, 2);

        InventorySlotLocation inventorySlotPosition(screenLocation, screenSize);
        inventorySlotLocations.push_back(inventorySlotPosition);
    }
    return inventorySlotLocations;
}

std::vector<InventorySlotLocation> generateInput1Output1InventoryLayout()
{
    std::vector<InventorySlotLocation> inventorySlotLocations;
    inventorySlotLocations.push_back({{-0.5f, 0.8f}, {0.1f, 0.1f}});
    inventorySlotLocations.push_back({{0.5f, 0.8f}, {0.1f, 0.1f}});
    return inventorySlotLocations;
}

void generateInventoryLayouts()
{
    inventoryLayouts.insert(std::make_pair(InventoryLayout::grid10x4Inventory, generategrid10x4InventoryLayout()));
    inventoryLayouts.insert(std::make_pair(InventoryLayout::output1Input1, generateInput1Output1InventoryLayout()));
}

std::vector<InventorySlotLocation>& getInventoryLayoutPositions(InventoryLayout inventoryLayout)
{
    return inventoryLayouts.at(inventoryLayout);   
}
