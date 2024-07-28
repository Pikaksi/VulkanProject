#pragma once

#include "ItemSlot.hpp"
#include "2dRendering/UIManager.hpp"

class PlayerInventory
{
public:
	void update(UIManager& uiManager);

private:
	DefaultWindow* defaultWindow;
	bool inventoryIsActive = false;

	void enableInventory(UIManager& uiManager);
	void disableInventory(UIManager& uiManager);
};