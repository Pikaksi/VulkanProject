#pragma once

#include <optional>

#include "InventoryLayouts.hpp"
#include "Inventory.hpp"
#include "2dRendering/UIManager.hpp"

void renderInventory(UIManager& uiManager, std::optional<int>& clickedSlot, std::optional<int>& howerOverSlot, Inventory& inventory, InventoryLayout inventoryLayout);