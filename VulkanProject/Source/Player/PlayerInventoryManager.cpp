#include "PlayerInventoryManager.hpp"

#include "PlayerInputHandler.hpp"

void PlayerInventoryManager::update(UIManager& uiManager)
{
	if (PlayerInputHandler::getInstance().rPressed) {
		if (inventoryIsActive) {
			inventoryIsActive = false;
			PlayerInputHandler::getInstance().disableCursor();
			previousClickedSlot = std::nullopt;
			std::cout << "disabled\n";
		}
		else {
			inventoryIsActive = true;
			PlayerInputHandler::getInstance().enableCursor();
			std::cout << "enabled\n";
		}
	}

	if (inventoryIsActive) {
		std::optional<int> clickedSlot, howerOverSlot;

		renderInventory(uiManager, clickedSlot, howerOverSlot, playerInventory, playerInventoryLayout);

		if (clickedSlot.has_value()) {
			if (previousClickedSlot.has_value()) {
				inventory.swapSlots(previousClickedSlot.value(), clickedSlot.value());
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
}
