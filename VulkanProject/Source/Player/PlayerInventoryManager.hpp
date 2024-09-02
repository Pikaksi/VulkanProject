#pragma once

#include <optional>

#include "ItemStack.hpp"
#include "2dRendering/UIManager.hpp"
#include "InventoryLayouts.hpp"
#include "Inventory.hpp"
#include "InventoryRenderer.hpp"
#include "ECS/EntityManager.hpp"

class PlayerInventoryManager
{
public:
	void update(UIManager& uiManager);
	PlayerInventoryManager() :
		playerInventory(Inventory(getInventoryLayoutSize(playerInventoryLayout)))
	{
		playerInventory.insertItem(ItemStack(Item::dirt, 3));
	}

private:
	bool inventoryIsActive = false;
	std::optional<int> previousClickedSlot = std::nullopt;
	
	const static InventoryLayout playerInventoryLayout = InventoryLayout::grid10x4Inventory;
	Inventory playerInventory;

	std::optional<EntityID> additionalOpenInventory;
};