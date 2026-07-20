#include "PlayerInventoryManager.hpp"

#include "PlayerInputHandler.hpp"

void PlayerInventoryManager::update(UIManager& uiManager)
{
	if (PlayerInputHandler::getInstance().rPressed) {
		if (inventoryIsActive) {
			closeInventory();
		}
		else {
			openInventory(std::nullopt);
		}
	}

	if (inventoryIsActive) {
		processOpenInventory(uiManager);
	}
}

void PlayerInventoryManager::openInventory(std::optional<EntityID> additionalInventoryEntityID)
{
	inventoryIsActive = true;
	PlayerInputHandler::getInstance().enableCursor();
	additionalOpenInventory = additionalInventoryEntityID;
}

void PlayerInventoryManager::closeInventory()
{
	inventoryIsActive = false;
	PlayerInputHandler::getInstance().disableCursor();
}

void PlayerInventoryManager::processOpenInventory(UIManager& uiManager)
{
	std::optional<int> clickedSlotPlayerInventory, howerOverSlotPlayerInventory, clickedSlotAdditionalInventory, howerOverSlotAdditionalInventory;

	renderInventory(
		uiManager, 
		clickedSlotPlayerInventory, 
		howerOverSlotPlayerInventory, 
		playerInventory, 
		playerInventoryLayout,
		true);
	SelectedSlotInfo selectedSlotInfo {
		clickedSlotPlayerInventory,
		std::nullopt
	};

	if (additionalOpenInventory.has_value()) {
		std::cout << "additional inventory\n";
		std::cout << "entityID = " << additionalOpenInventory.value() << "\n";
		renderInventory(
			uiManager, 
			clickedSlotAdditionalInventory, 
			howerOverSlotAdditionalInventory, 
			entityManager.entities[additionalOpenInventory.value()].getComponent<Inventory>(), 
			InventoryLayout::output1Input1,
			false);

		if (clickedSlotAdditionalInventory.has_value()) {
			selectedSlotInfo.slotNumber = clickedSlotAdditionalInventory;
			selectedSlotInfo.inventoryEntityID = additionalOpenInventory;
		}
	}

	handleClickedSlot(selectedSlotInfo);
}

void PlayerInventoryManager::handleClickedSlot(SelectedSlotInfo selectedSlotInfo)
{
	if (!selectedSlotInfo.slotNumber.has_value()) {
		return;
	}

	Inventory& otherInventory = 
		selectedSlotInfo.inventoryEntityID.has_value()
		? entityManager.entities[selectedSlotInfo.inventoryEntityID.value()].getComponent<Inventory>()
		: playerInventory;

	swapSlots(0, selectedSlotInfo.slotNumber.value(), cursorInventory, otherInventory);
}

