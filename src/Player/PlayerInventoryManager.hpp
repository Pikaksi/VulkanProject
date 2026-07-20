#pragma once

#include <optional>

#include "Inventory/ItemStack.hpp"
#include "Inventory/InventoryLayouts.hpp"
#include "Inventory/Inventory.hpp"
#include "Inventory/InventoryRenderer.hpp"
#include "2dRendering/UIManager.hpp"
#include "ECS/EntityManager.hpp"

struct SelectedSlotInfo
{
	std::optional<int> slotNumber;
	std::optional<EntityID> inventoryEntityID; // is player inventory if there is no value
};

class PlayerInventoryManager
{
public:
	void update(UIManager& uiManager);
	void openInventory(std::optional<EntityID> additionalInventoryEntityID = std::nullopt);

	PlayerInventoryManager() :
		playerInventory(Inventory(getInventoryLayoutSize(playerInventoryLayout))),
		cursorInventory(Inventory(1))
	{
		playerInventory.insertItem(ItemStack(Item::dirt, 3));
	}

private:
	void closeInventory();
	void processOpenInventory(UIManager& uiManager);
	void handleClickedSlot(SelectedSlotInfo selectedSlotInfo);

	const static InventoryLayout playerInventoryLayout = InventoryLayout::grid10x4Inventory;
	bool inventoryIsActive = false;
	std::optional<EntityID> additionalOpenInventory;
	
	Inventory playerInventory;
	Inventory cursorInventory;
};