#pragma once

#include <optional>

#include "ItemStack.hpp"
#include "2dRendering/UIManager.hpp"
#include "InventoryLayouts.hpp"
#include "Inventory.hpp"
#include "InventoryRenderer.hpp"
#include "ECS/EntityManager.hpp"

struct SelectedSlotInfo
{
	int slotNumber;
	bool isPlayerInventory;
	EntityID inventoryEntityID; // if is not playerInventory
};

class PlayerInventoryManager
{
public:
	void update(UIManager& uiManager);
	void openInventory(std::optional<EntityID> additionalInventoryEntityID = std::nullopt);

	PlayerInventoryManager() :
		playerInventory(Inventory(getInventoryLayoutSize(playerInventoryLayout)))
	{
		playerInventory.insertItem(ItemStack(Item::dirt, 3));
	}

private:
	void closeInventory();
	void processOpenInventory(UIManager& uiManager);

	bool inventoryIsActive = false;
	std::optional<SelectedSlotInfo> previousClickedSlot = std::nullopt;
	std::optional<EntityID> additionalOpenInventory;
	
	const static InventoryLayout playerInventoryLayout = InventoryLayout::grid10x4Inventory;
	Inventory playerInventory;
};