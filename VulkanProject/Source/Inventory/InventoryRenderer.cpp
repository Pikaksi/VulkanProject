#include "InventoryRenderer.hpp"

#include <chrono>

#include "2dRendering/ItemToTexLayer.hpp"
#include "2dRendering/UIHelperFunctions.hpp"
#include "PlayerInputHandler.hpp"
#include "2dRendering/DefaultWindow.hpp"

void renderInventory(UIManager& uiManager, std::optional<int>& clickedSlot, std::optional<int>& howerOverSlot, Inventory& inventory, InventoryLayout inventoryLayout)
{
    auto startingTime = std::chrono::high_resolution_clock::now();

    clickedSlot = std::nullopt;
    howerOverSlot = std::nullopt;
    glm::vec2 windowLocation = {0.0f, 0.0f};
    glm::vec2 windowSize = {1.0f, 1.0f}; 
    float topBarHeight = 0.05f;

    windowSize *= uiManager.getScalar();
    centerLocation(windowLocation, windowSize, UICenteringMode::center);

	createDefaultWindow(
        uiManager,
		windowLocation,
		windowSize,
		topBarHeight,
		"Inventory",
		{0.1f, 0.2, 0.8f, 1.0f},
		{0.05f, 0.05f, 0.07f, 1.0f});

    glm::vec2 windowBodyLocation = windowLocation + glm::vec2(0.0f, topBarHeight);
    glm::vec2 windowBodySize = windowSize - glm::vec2(0.0f, topBarHeight);
    std::vector<InventorySlotLocation>& inventorySlotLocations = getInventoryLayoutPositions(inventoryLayout);

    glm::vec2 testLocation = {-1.0f, -1.0f};
    glm::vec2 testSize = {0.5f, 0.5f};
    scaleBoxToWindow(windowBodyLocation, windowBodySize, testLocation, testSize);
    createUIQuad(
        uiManager,
        testLocation,
        testSize,
        {0.0f, 0.0f},
        {1.0f, 1.0f},
        UITexLayer::white,
        {1.0f, 1.0f, 1.0f, 1.0f});

    std::cout << "window location = " << windowBodyLocation.x << " " << windowBodyLocation.y << "\n";
    std::cout << "window size = " << windowBodySize.x << " " << windowBodySize.y << "\n";

    for (int i = 0; i < inventorySlotLocations.size(); i++) {
        InventorySlotLocation slotLocation = inventorySlotLocations[i];
        scaleBoxToWindow(windowBodyLocation, windowBodySize, slotLocation);
        std::cout << "slot location = " << slotLocation.location.x << " " << slotLocation.location.y << "\n";

        createUIQuad(
            uiManager,
            slotLocation.location,
            slotLocation.size,
            { 0.0f, 0.0f },
            { 1.0f, 1.0f },
            UITexLayer::white,
            {0.2f, 0.2f, 0.2f, 1.0f});

        glm::vec2 mouseLocation = {PlayerInputHandler::getInstance().mouseLocationX, PlayerInputHandler::getInstance().mouseLocationY};
        bool mouseInSlot = isLocationInBox(mouseLocation, slotLocation.location, slotLocation.size);
        
        if (mouseInSlot) {
            howerOverSlot = i;

            if (PlayerInputHandler::getInstance().mouseLeftPressed) {
                clickedSlot = i;
            }
        }

        ItemStack itemStackInSlot = inventory.getItem(i);

        if (itemStackInSlot.item != Item::empty) {
            UITexLayer itemTexLayer = itemToUITexLayer.at(itemStackInSlot.item);

            createUIQuad(
                uiManager,
                slotLocation.location,
                slotLocation.size - 0.02f,
                { 0.0f, 0.0f },
                { 1.0f, 1.0f },
                itemTexLayer,
                {1.0f, 1.0f, 1.0f, 1.0f});

            glm::vec2 slotBottomRight = getCenteredLocation(slotLocation.location, slotLocation.size, UICenteringMode::bottomLeft);

            createUIText(
                uiManager,
                slotLocation.location,
                slotLocation.size.y / 2.0f,
                UICenteringMode::topLeft,
                UICenteringMode::topLeft,
                std::to_string(itemStackInSlot.amount));
        }
    }

    auto endingTime = std::chrono::high_resolution_clock::now();
    auto timeTaken = std::chrono::duration_cast<std::chrono::nanoseconds>(endingTime - startingTime).count();
    //std::cout << "time taken to generate inventory UIObjects in nanoseconds is " << timeTaken << "\n";
}