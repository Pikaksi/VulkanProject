#pragma once

#include <functional>

#include "InventoryLayouts.hpp"
#include "Inventory.hpp"
#include "2dRendering/UIManager.hpp"

class InventoryRenderer
{
public:
	void enableInventory(UIManager& uiManager, std::function<void(int)> inventorySlotCallback, Inventory* inventory);
	void disableInventory(UIManager& uiManager);

	InventoryRenderer(InventoryLayout inventoryLayout)
	    : inventoryLayout(inventoryLayout) {}

private:
	InventoryLayout inventoryLayout;

	DefaultWindow* defaultWindow;
	std::vector<UIButton*> buttons;
	std::vector<UIQuad*> buttonImages;
	std::vector<UIQuad*> itemImages;
	std::vector<UIText*> itemCounts;
};