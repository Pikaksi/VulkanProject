#pragma once

#include "ItemStack.hpp"
#include "2dRendering/UIManager.hpp"
#include "InventoryLayouts.hpp"
#include "Inventory.hpp"
#include "InventoryRenderer.hpp"

class PlayerInventory
{
public:
	void update(UIManager& uiManager);
	PlayerInventory()
		: inventory(Inventory(inventorySize)), inventoryRenderer(InventoryRenderer(&inventory, inventoryLayout)) {}

private:
	InventoryLayout inventoryLayout = InventoryLayout::grid10x4Inventory;	
	uint32_t inventorySize = getInventoryLayoutSize(inventoryLayout);

	Inventory inventory;
	InventoryRenderer inventoryRenderer;

	bool inventoryIsActive = false;

	void enableInventory(UIManager& uiManager);
	void disableInventory(UIManager& uiManager);

	void testCallback(int callbackNumber);
};