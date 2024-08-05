#pragma once

#include <functional>

#include "InventoryLayouts.hpp"
#include "Inventory.hpp"
#include "2dRendering/UIManager.hpp"

class InventoryRenderer
{
public:
	void enableInventory(UIManager& uiManager, std::function<void(int)> inventorySlotCallback);
	void disableInventory(UIManager& uiManager);

	void testCallback(int callbackNumber);

	InventoryRenderer(Inventory* inventory, InventoryLayout inventoryLayout)
	    : inventory(inventory), inventoryLayout(inventoryLayout) {}

private:
	Inventory* inventory;
	InventoryLayout inventoryLayout;

	DefaultWindow* defaultWindow;
	std::vector<UIButton*> buttons;
	std::vector<UIQuad*> buttonImages;

	bool inventoryIsActive = false;
};