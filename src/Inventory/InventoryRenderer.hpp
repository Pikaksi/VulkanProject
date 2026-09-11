#pragma once

#include <optional>

#include "InventoryLayouts.hpp"
#include "Inventory.hpp"
#include "2dRendering/UIManager.hpp"

void renderInventory(UIManager& uiManager,
                     std::optional<int>& clickedSlot,
                     std::optional<int>& howerOverSlot,
                     Inventory& inventory,
                     InventoryLayout inventoryLayout,
                     bool renderWindow,
                     bool playerClicled);

void renderSingleItem(UIManager& uiManager,
                      ItemStack itemStack,
                      bool renderItemAmount,
                      glm::vec2 location,
                      glm::vec2 size);
