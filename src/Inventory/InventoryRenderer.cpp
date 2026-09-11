#include "InventoryRenderer.hpp"

#include <cassert>
#include <chrono>
#include <string>

#include "2dRendering/ItemToTexLayer.hpp"
#include "2dRendering/UIHelperFunctions.hpp"
#include "InventoryLayouts.hpp"
#include "PlayerInputHandler.hpp"
#include "2dRendering/DefaultWindow.hpp"
#include "UIQuad.hpp"
#include "assertm.hpp"

void renderInventory(UIManager& uiManager,
                     std::optional<int>& clickedSlot,
                     std::optional<int>& howerOverSlot,
                     Inventory& inventory,
                     InventoryLayout inventoryLayout,
                     bool renderWindow,
                     bool playerClicled)
{
    assertm(inventory.itemStacks.size() != 0, "Inventory size is 0");
    assertm(inventory.itemStacks.size() == getInventoryLayoutPositions(inventoryLayout).size(),
            "Inventory layout and invenotry sizes dont match");

    auto startingTime = std::chrono::high_resolution_clock::now();

    clickedSlot = std::nullopt;
    howerOverSlot = std::nullopt;
    glm::vec2 windowLocation = {0.0f, 0.0f};
    glm::vec2 windowSize = glm::vec2{1.6f, 1.6f} * uiManager.scalar;
    float topBarHeight = 0.05f;

    centerLocation(windowLocation, windowSize, UICenteringMode::center);

    if (renderWindow) {
        createDefaultWindow(uiManager,
                            windowLocation,
                            windowSize,
                            topBarHeight,
                            "Inventory",
                            {0.1f, 0.2, 0.8f, 1.0f},
                            {0.05f, 0.05f, 0.07f, 1.0f});
    }

    glm::vec2 windowBodyLocation = windowLocation + glm::vec2(0.0f, topBarHeight);
    glm::vec2 windowBodySize = windowSize - glm::vec2(0.0f, topBarHeight);
    std::vector<InventorySlotLocation>& inventorySlotLocations = getInventoryLayoutPositions(inventoryLayout);

    assertm(inventorySlotLocations.size() == inventory.itemStacks.size(),
            "Inventory size and layout size do not match");

    glm::vec2 mouseLocation = uiManager.getMousePositionScreenSpace();
    for (int i = 0; i < inventorySlotLocations.size(); i++) {
        InventorySlotLocation slotLocation = inventorySlotLocations[i];
        scaleBoxToWindow(windowBodyLocation, windowBodySize, slotLocation);

        createUIQuad(uiManager,
                     slotLocation.location,
                     slotLocation.size,
                     {0.0f, 0.0f},
                     {1.0f, 1.0f},
                     UITexLayer::white,
                     {0.2f, 0.2f, 0.2f, 1.0f});

        bool mouseInSlot = isLocationInBox(mouseLocation, slotLocation.location, slotLocation.size);

        if (mouseInSlot) {
            howerOverSlot = i;

            if (playerClicled) {
                clickedSlot = i;
            }
        }
        ItemStack itemStackInSlot = inventory.itemStacks[i];

        if (itemStackInSlot.item != Item::empty) {
            UITexLayer itemTexLayer = itemToUITexLayer.at(itemStackInSlot.item);

            slotLocation.location += slotLocation.size / 20.0f;
            slotLocation.size *= 0.9f;

            createUIQuad(uiManager,
                         slotLocation.location,
                         slotLocation.size,
                         {0.0f, 0.0f},
                         {1.0f, 1.0f},
                         itemTexLayer,
                         {1.0f, 1.0f, 1.0f, 1.0f});

            std::string itemCountText = std::to_string(itemStackInSlot.amount);
            glm::vec2 textSize = {slotLocation.size.y / 2.0f, slotLocation.size.y / 4.0f * itemCountText.size()};
            glm::vec2 itemCountLocation =
                getCenteredLocation(slotLocation.location, slotLocation.size, UICenteringMode::bottomLeft);
            centerLocation(itemCountLocation, textSize, UICenteringMode::topLeft);

            createUIText(uiManager,
                         slotLocation.location,
                         slotLocation.size.y / 2.0f,
                         UICenteringMode::topLeft,
                         UICenteringMode::topLeft,
                         itemCountText);
        }
    }

    auto endingTime = std::chrono::high_resolution_clock::now();
    auto timeTaken = std::chrono::duration_cast<std::chrono::nanoseconds>(endingTime - startingTime).count();
    // std::cout << "time taken to generate inventory UIObjects in nanoseconds is " << timeTaken << "\n";
}

void renderSingleItem(
    UIManager& uiManager, ItemStack itemStack, bool renderItemAmount, glm::vec2 location, glm::vec2 size)
{
    assertm(itemStack.item != Item::empty, "Tried to render single empty item");
    UITexLayer itemTexLayer = itemToUITexLayer.at(itemStack.item);

    size *= uiManager.scalar;
    createUIQuad(uiManager, location, size, {0.0f, 0.0f}, {1.0f, 1.0f}, itemTexLayer, {1.0f, 1.0f, 1.0f, 1.0f});

    if (renderItemAmount) {
        createUIText(uiManager,
                     location,
                     size.y / 2.0f,
                     UICenteringMode::topLeft,
                     UICenteringMode::topLeft,
                     std::to_string(itemStack.amount));
    }
}
