#include "PlayerInventoryManager.hpp"

#include "PlayerInputHandler.hpp"

void PlayerInventoryManager::update(UIManager& uiManager)
{
	if (PlayerInputHandler::getInstance().rPressed) {
		if (inventoryIsActive) {
			closeInventory();
		}
		else {
			openInventory();
		}
	}

	if (inventoryIsActive) {
		processOpenInventory(uiManager);
	}
}

void PlayerInventoryManager::openInventory(std::optional<EntityID> additionalInventoryEntityID = std::nullopt)
{
	inventoryIsActive = true;
	PlayerInputHandler::getInstance().enableCursor();
}

void PlayerInventoryManager::closeInventory()
{
	inventoryIsActive = false;
	PlayerInputHandler::getInstance().disableCursor();
	previousClickedSlot = std::nullopt;
}

void PlayerInventoryManager::processOpenInventory(UIManager& uiManager)
{
	std::optional<int> clickedSlot, howerOverSlot;

	renderInventory(uiManager, clickedSlot, howerOverSlot, playerInventory, playerInventoryLayout);

	if (additionalOpenInventory.has_value()) {
		renderInventory(uiManager, clickedSlot, howerOverSlot, entityManager.entities[additionalOpenInventory.value()].getComponent<Inventory>(), )
	}

	if (clickedSlot.has_value()) {
		if (previousClickedSlot.has_value()) {
			playerInventory.swapSlots(previousClickedSlot.value(), clickedSlot.value());
			previousClickedSlot = std::nullopt;
		}
		else {
			previousClickedSlot = clickedSlot.value();
		}
		std::cout << "Clicked slot " << clickedSlot.value() << "\n";
	}
	if (howerOverSlot.has_value()) {
	}
}
