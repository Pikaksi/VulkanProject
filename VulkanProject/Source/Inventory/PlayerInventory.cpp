#include "PlayerInventory.hpp"

#include "PlayerInputHandler.hpp"

void PlayerInventory::update(UIManager& uiManager)
{
	if (PlayerInputHandler::getInstance().rPressed) {
		if (inventoryIsActive) {
			inventoryIsActive = false;
			PlayerInputHandler::getInstance().disableCursor();
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

		renderInventory(uiManager, clickedSlot, howerOverSlot, inventory, inventoryLayout);

		if (clickedSlot.has_value()) {
			
		}
	}
}
