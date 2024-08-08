#include "InventoryRenderer.hpp"

#include "2dRendering/ItemToTexLayer.hpp"
#include "2dRendering/UIHelperFunctions.hpp"
#include "PlayerInputHandler.hpp"

void InventoryRenderer::enableInventory(UIManager& uiManager, std::function<void(int)> inventorySlotCallback, Inventory* inventory)
{
	defaultWindow = uiManager.createDefaultWindow(
		{0.0f, 0.0f},
		{1.2f, 1.2f},
		UICenteringMode::center,
		0.05f,
		"Inventory",
		{0.1f, 0.2, 0.8f, 1.0f},
		{0.05f, 0.05f, 0.07f, 1.0f});
	uiManager.updateUIObject(defaultWindow);

    std::vector<InventorySlotLocation>& inventorySlotLocations = getInventoryLayoutPositions(inventoryLayout);

    for (int i = 0; i < inventorySlotLocations.size(); i++) {
        InventorySlotLocation slotLocation = inventorySlotLocations[i];
        scaleBoxToWindow(defaultWindow->location, defaultWindow->size, slotLocation.location, slotLocation.size);

        buttons.push_back(uiManager.createUIButton(
            slotLocation.location,
            slotLocation.size,
            UICenteringMode::topLeft,
            true,
            inventorySlotCallback,  // create the callback function like this: std::bind(&PlayerInventory::testCallback, this, std::placeholders::_1));
            i));

        buttonImages.push_back(uiManager.createUIQuad(
            slotLocation.location,
            slotLocation.size,
            { 0.0f, 0.0f },
            { 1.0f, 1.0f },
            UITexLayer::white,
            {0.2f, 0.2f, 0.2f, 1.0f},
            UICenteringMode::topLeft,
            true));
        uiManager.updateUIObject(buttonImages.back());

        ItemStack itemStackInSlot = inventory->getItem(i);

        if (itemStackInSlot.item != Item::empty) {
            UITexLayer itemTexLayer = itemToUITexLayer.at(itemStackInSlot.item);

            itemImages.push_back(uiManager.createUIQuad(
                slotLocation.location + 0.01f,
                slotLocation.size - 0.02f,
                { 0.0f, 0.0f },
                { 1.0f, 1.0f },
                itemTexLayer,
                {1.0f, 1.0f, 1.0f, 1.0f},
                UICenteringMode::topLeft,
                true));
            uiManager.updateUIObject(itemImages.back());

            itemCounts.push_back(uiManager.createUIText(
                slotLocation.location + slotLocation.size,
                slotLocation.size.y / 2.0f,
                std::to_string(itemStackInSlot.amount),
                UICenteringMode::bottomRight
            ));
            uiManager.updateUIObject(itemCounts.back());
        }
    }

	PlayerInputHandler::getInstance().enableCursor();
}

void InventoryRenderer::disableInventory(UIManager& uiManager)
{
	uiManager.destroyUIObject(defaultWindow);

    for (int i = 0; i < buttons.size(); i++) {
        uiManager.destroyUIButton(buttons[i]);
    }
    buttons.clear();
    for (int i = 0; i < buttonImages.size(); i++) {
        uiManager.destroyUIObject(buttonImages[i]);
    }
    buttonImages.clear();
    for (int i = 0; i < itemImages.size(); i++) {
        uiManager.destroyUIObject(itemImages[i]);
    }
    itemImages.clear();
    for (int i = 0; i < itemCounts.size(); i++) {
        uiManager.destroyUIObject(itemCounts[i]);
    }
    itemCounts.clear();

	PlayerInputHandler::getInstance().disableCursor();
}