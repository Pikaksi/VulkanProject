#pragma once

#include <optional>

#include "Inventory/ItemStack.hpp"
#include "Inventory/InventoryLayouts.hpp"
#include "Inventory/Inventory.hpp"
#include "Inventory/InventoryRenderer.hpp"
#include "2dRendering/UIManager.hpp"
#include "blockEntityManager.hpp"
#include "playerInfo.hpp"

struct SelectedSlotInfo
{
    std::optional<int> slotNumber;
    std::optional<BlockEntityId> inventoryEntityID; // is player inventory if there is no value
};

/*struct PlayerInventoryManager
{
    PlayerInventoryManager()
    {
    }

    void update(UIManager& uiManager, BlockEntityManager& blockEntityManager);
    void openInventory(std::optional<BlockEntityId> additionalInventoryEntityID,
                       BlockEntityManager& blockEntityManager);
    void closeInventory();
    void processOpenInventory(UIManager& uiManager, BlockEntityManager& blockEntityManager);
    void handleClickedSlot(SelectedSlotInfo selectedSlotInfo);
};*/
