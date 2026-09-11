#pragma once

#include <optional>

#include "Inventory/ItemStack.hpp"
#include "Inventory/InventoryLayouts.hpp"
#include "Inventory/Inventory.hpp"
#include "Inventory/InventoryRenderer.hpp"
#include "2dRendering/UIManager.hpp"
#include "blockEntityManager.hpp"
#include "playerInfo.hpp"

void updatePlayerInventory(PlayerInfo& playerInfo, UIManager& uiManager, BlockEntityManager& blockEntityManager);
