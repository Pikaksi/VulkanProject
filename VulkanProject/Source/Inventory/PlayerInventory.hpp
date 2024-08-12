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
	PlayerInventory() :
		inventoryLayout(InventoryLayout::grid10x4Inventory),
		inventorySize(getInventoryLayoutSize(inventoryLayout)), 
		inventory(Inventory(inventorySize))
	{
		inventory.insertItem(ItemStack(Item::dirt, 3));
	}

private:
	InventoryLayout inventoryLayout;
	uint32_t inventorySize;
	Inventory inventory;
	bool inventoryIsActive = false;
};