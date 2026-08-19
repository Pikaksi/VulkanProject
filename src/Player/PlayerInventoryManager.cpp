#include "PlayerInventoryManager.hpp"

#include "Inventory.hpp"
#include "PlayerInputHandler.hpp"

void update(PlayerInfo& playerInfo, UIManager& uiManager, BlockEntityManager& blockEntityManager)
{
    if (PlayerInputHandler::getInstance().rPressed) {
        if (playerInfo.inventoryIsActive) {
            closeInventory();
        }
        else {
            openInventory(std::nullopt, blockEntityManager);
        }
    }

    if (inventoryIsActive) {
        processOpenInventory(uiManager, blockEntityManager);
    }
}

void openInventory(PlayerInfo& playerInfo)
{
    playerInfo.inventoryIsActive = true;
    PlayerInputHandler::getInstance().enableCursor();
}

void closeInventory(PlayerInfo& playerInfo)
{
    playerInfo.inventoryIsActive = false;
    PlayerInputHandler::getInstance().disableCursor();
}

void processOpenInventory(UIManager& uiManager, BlockEntityManager& blockEntityManager)
{
    std::optional<int> clickedSlotPlayerInventory, howerOverSlotPlayerInventory, clickedSlotAdditionalInventory,
        howerOverSlotAdditionalInventory;

    renderInventory(uiManager,
                    clickedSlotPlayerInventory,
                    howerOverSlotPlayerInventory,
                    playerInventory,
                    playerInventoryLayout,
                    true);
    SelectedSlotInfo selectedSlotInfo{clickedSlotPlayerInventory, std::nullopt};

    if (additionalOpenInventory.has_value()) {
        std::cout << "additional inventory\n";
        std::cout << "entityID = " << additionalOpenInventory.value() << "\n";

        BlockEntity* entity = blockEntityManager.entities[additionalOpenInventory.value()];
        Inventory

            renderInventory(uiManager,
                            clickedSlotAdditionalInventory,
                            howerOverSlotAdditionalInventory,

                            // entityManager.entities[additionalOpenInventory.value()].getComponent<Inventory>(),
                            InventoryLayout::output1Input1,
                            false);

        if (clickedSlotAdditionalInventory.has_value()) {
            selectedSlotInfo.slotNumber = clickedSlotAdditionalInventory;
            selectedSlotInfo.inventoryEntityID = additionalOpenInventory;
        }
    }

    handleClickedSlot(selectedSlotInfo);
}

void handleClickedSlot(SelectedSlotInfo selectedSlotInfo)
{
    if (!selectedSlotInfo.slotNumber.has_value()) {
        return;
    }

    Inventory& otherInventory =
        selectedSlotInfo.inventoryEntityID.has_value()
            ? entityManager.entities[selectedSlotInfo.inventoryEntityID.value()].getComponent<Inventory>()
            : playerInventory;

    swapSlots(0, selectedSlotInfo.slotNumber.value(), cursorInventory, otherInventory);
}
