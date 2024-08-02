#pragma once

#include "ItemSlot.hpp"
#include "2dRendering/UIManager.hpp"

class PlayerInventory
{
public:
	void update(UIManager& uiManager);

private:
	DefaultWindow* defaultWindow;
	UIButton* button;
	UIQuad* buttonImage;

	bool inventoryIsActive = false;

	void enableInventory(UIManager& uiManager);
	void disableInventory(UIManager& uiManager);

	void testCallback(int callbackNumber);
};