#include "PlayerInventory.hpp"

#include "PlayerInputHandler.hpp"

void PlayerInventory::update(UIManager& uiManager)
{
	if (PlayerInputHandler::getInstance().rPressed) {
		if (inventoryIsActive) {
			inventoryIsActive = false;
			disableInventory(uiManager);
		}
		else {
			inventoryIsActive = true;
			enableInventory(uiManager);
		}
	}
}

void PlayerInventory::enableInventory(UIManager& uiManager)
{
	inventoryRenderer.enableInventory(uiManager, std::bind(&PlayerInventory::testCallback, this, std::placeholders::_1));
}

void PlayerInventory::testCallback(int callbackNumber)
{
	std::cout << "test callback " << callbackNumber << "\n";
}

void PlayerInventory::disableInventory(UIManager& uiManager)
{
	inventoryRenderer.disableInventory(uiManager);
}