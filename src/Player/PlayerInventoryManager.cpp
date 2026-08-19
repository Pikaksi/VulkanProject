#include "PlayerInventoryManager.hpp"

#include "Inventory.hpp"
#include "PlayerInputHandler.hpp"
#include "blockEntity.hpp"
#include "assertm.hpp"
#include <fcntl.h>

struct SelectedSlotInfo
{
    bool slotSelected;
    bool slotInPlayerInventory;
    int slotNumber;
    BlockEntityId id;
};

void updateActiveInventory(PlayerInfo& playerInfo, UIManager& uiManager, BlockEntityManager& blockEntityManager)
{
    std::optional<int> clickedSlotPlayer;
    std::optional<int> howerOverSlotPlayer;

    std::optional<int> clickedSlotOther;
    std::optional<int> howerOverSlotOther;
    SelectedSlotInfo selectedSlotInfo{
        .slotSelected = false,
    };

    renderInventory(uiManager,
                    clickedSlotPlayer,
                    howerOverSlotPlayer,
                    playerInfo.playerInventory,
                    playerInfo.playerInventoryLayout,
                    true);

    if (clickedSlotPlayer.has_value()) {
        selectedSlotInfo = SelectedSlotInfo{
            .slotSelected = true,
            .slotInPlayerInventory = true,
            .slotNumber = clickedSlotPlayer.value(),
        };
    }

    if (blockEntityManager.blockEntityLocations.contains(playerInfo.lookingAtLocationWorld)) {
        BlockEntityId id = blockEntityManager.blockEntityLocations.at(playerInfo.lookingAtLocationWorld);
        BlockEntity* entity = blockEntityManager.entities[id];
        Inventory* inventory = nullptr;
        bool hasInventory = tryGetInventory(entity, inventory);
        if (hasInventory) {
            renderInventory(
                uiManager, clickedSlotOther, howerOverSlotOther, *inventory, InventoryLayout::output1Input1, false);

            if (clickedSlotOther.has_value()) {
                selectedSlotInfo = SelectedSlotInfo{
                    .slotSelected = true,
                    .slotInPlayerInventory = false,
                    .slotNumber = clickedSlotOther.value(),
                    .id = id,
                };
            }
        }
    }

    if (selectedSlotInfo.slotSelected) {
        Inventory* inventory = nullptr;
        if (selectedSlotInfo.slotInPlayerInventory) {
            inventory = &playerInfo.playerInventory;
        }
        else {
            bool success = tryGetInventory(blockEntityManager.entities[selectedSlotInfo.id], inventory);
            assertm(success, "Failed to get inventory");
        }

        swapSlots(0, selectedSlotInfo.slotNumber, playerInfo.cursorInventory, *inventory);
    }
}

void updatePlayerInventory(PlayerInfo& playerInfo, UIManager& uiManager, BlockEntityManager& blockEntityManager)
{
    bool openBlockInventory = false;
    if (playerInfo.interactedWithBlock &&
        blockEntityManager.blockEntityLocations.contains(playerInfo.lookingAtLocationWorld)) {
        Inventory* inventory;
        BlockEntityId id = blockEntityManager.blockEntityLocations.at(playerInfo.lookingAtLocationWorld);
        openBlockInventory = tryGetInventory(blockEntityManager.entities[id], inventory);

        std::cout << "looking at: " << playerInfo.lookingAtLocationWorld.x << " " << playerInfo.lookingAtLocationWorld.y << " " << playerInfo.lookingAtLocationWorld.z << std::endl;
        std::cout << "locations are:" << std::endl;
        for (auto& pair : blockEntityManager.blockEntityLocations) {
            std::cout << "  " << pair.first.x << " " << pair.first.y << " " << pair.first.z << " type: " << (int)blockEntityManager.entities[pair.second]->type << std::endl;
        }
        std::cout << "opening block inventory: " << openBlockInventory
                  << " from block type: " << (int)blockEntityManager.entities[id]->type << std::endl;

        playerInfo.interactedWithBlock = false;
    }

    if (PlayerInputHandler::getInstance().rPressed || openBlockInventory) {
        if (playerInfo.inventoryIsActive) {
            playerInfo.inventoryIsActive = false;
            PlayerInputHandler::getInstance().disableCursor();
        }
        else {
            playerInfo.inventoryIsActive = true;
            PlayerInputHandler::getInstance().enableCursor();
        }
    }

    if (playerInfo.inventoryIsActive) {
        updateActiveInventory(playerInfo, uiManager, blockEntityManager);
    }
}
