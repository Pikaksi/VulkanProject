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
	defaultWindow = uiManager.createDefaultWindow(
		{0.0f, 0.0f},
		{1.2f, 0.8f},
		UICenteringMode::center,
		0.05f,
		"Inventory",
		{0.1f, 0.2, 0.8f, 1.0f},
		{0.05f, 0.05f, 0.07f, 1.0f});
	uiManager.updateUIObject(defaultWindow);

	button = uiManager.createUIButton(
		glm::vec2(0.8f, 0.8f),
		glm::vec2(0.2f, 0.2f),
		UICenteringMode::topLeft,
		true,
		std::bind(&PlayerInventory::testCallback, this, 69));

	buttonImage = uiManager.createUIQuad(
		{ 0.8f, 0.8f },
		{ 0.2f, 0.2f },
		{ 0.0f, 0.0f },
		{ 1.0f, 1.0f },
		UITexLayer::white,
		{0.2f, 0.2f, 0.2f, 1.0f},
		UICenteringMode::topLeft,
		true);
	uiManager.updateUIObject(buttonImage);

	PlayerInputHandler::getInstance().enableCursor();
}

void PlayerInventory::testCallback(int callbackNumber)
{
	std::cout << "test callback " << callbackNumber << "\n";
}

void PlayerInventory::disableInventory(UIManager& uiManager)
{
	uiManager.destroyUIObject(defaultWindow);

	PlayerInputHandler::getInstance().disableCursor();
}