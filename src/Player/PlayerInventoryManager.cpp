#include "PlayerInventoryManager.hpp"

#include <GL/glext.h>
#include <algorithm>

#include "Constants.hpp"
#include "Inventory.hpp"
#include "InventoryRenderer.hpp"
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

    bool playerClicked =
        inputHandler.mousePressed(GLFW_MOUSE_BUTTON_LEFT) || inputHandler.mousePressed(GLFW_MOUSE_BUTTON_RIGHT);

    assertm(playerInfo.playerInventory.itemStacks.size() != 0, "Player inventory has size 0");
    renderInventory(uiManager,
                    clickedSlotPlayer,
                    howerOverSlotPlayer,
                    playerInfo.playerInventory,
                    playerInfo.playerInventoryLayout,
                    true,
                    playerClicked);

    glm::vec2 mouseLocation = uiManager.getMousePositionScreenSpace();
    if (playerInfo.cursorInventory.itemStacks[0].item != Item::empty) {
        renderSingleItem(
            uiManager, playerInfo.cursorInventory.itemStacks[0], true, mouseLocation, glm::vec2{0.07, 0.07});
    }

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
            assertm(inventory != nullptr, "Block entity owns null inventory");
            assertm(inventory->itemStacks.size() != 0, "0 size");
            InventoryLayout layout = blockEntityToInventoryLayout[(size_t)entity->type];
            renderInventory(uiManager,
                            clickedSlotOther,
                            howerOverSlotOther,
                            *inventory,
                            layout,
                            false,
                            playerClicked);

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
        ItemStack& itemInInventory = inventory->itemStacks[selectedSlotInfo.slotNumber];
        ItemStack& itemInCursor = playerInfo.cursorInventory.itemStacks[0];

        assertm(itemInCursor.item != Item::empty || itemInCursor.amount == 0,
                "Player cursor has empty item with nonzero count");

        // place items from cursor to stack
        if (inputHandler.mousePressed(GLFW_MOUSE_BUTTON_RIGHT)) {
            if (itemInCursor.item == itemInInventory.item && itemInInventory.amount < ITEM_STACK_MAX_SIZE) {

                int amount = std::min(itemInCursor.amount, ITEM_STACK_MAX_SIZE - itemInInventory.amount);
                itemInCursor.amount -= amount;
                if (itemInCursor.amount == 0) {
                    itemInCursor.item = Item::empty;
                }
                itemInInventory.amount += amount;
            }
            else {
                std::swap(itemInCursor, itemInInventory);
            }
        }
        else if (inputHandler.mousePressed(GLFW_MOUSE_BUTTON_RIGHT)) {
            if (itemInCursor.item == Item::empty) {
                int amount = (int)std::ceil((double)itemInInventory.amount / 2.0);
                itemInCursor.amount += amount;
                itemInCursor.item = itemInInventory.item;
                itemInInventory.amount -= amount;
                if (itemInInventory.amount == 0) {
                    itemInInventory.item = Item::empty;
                }
            }
            else if (itemInCursor.item != Item::empty && itemInInventory.item == Item::empty) {
                itemInInventory.amount = 1;
                itemInInventory.item = itemInCursor.item;

                itemInCursor.amount -= 1;
                if (itemInCursor.amount == 0) {
                    itemInCursor.item = Item::empty;
                }
            }
            else if (itemInCursor.item != Item::empty && itemInCursor.item == itemInInventory.item &&
                     itemInInventory.amount < ITEM_STACK_MAX_SIZE) {

                itemInCursor.amount -= 1;
                if (itemInCursor.amount == 0) {
                    itemInCursor.item = Item::empty;
                }
                itemInInventory.amount += 1;
            }
        }

        // swapSlots(0, selectedSlotInfo.slotNumber, playerInfo.cursorInventory, *inventory);
    }
}

void updatePlayerInventory(PlayerInfo& playerInfo, UIManager& uiManager, BlockEntityManager& blockEntityManager)
{
    for (int key = GLFW_KEY_0; key <= GLFW_KEY_9; key++) {
        if (inputHandler.keyPressed(key)) {
            playerInfo.selectedHotbarSlot = key - GLFW_KEY_0;
            break;
        }
    }

    bool openBlockInventory = false;
    if (playerInfo.interactedWithBlock &&
        blockEntityManager.blockEntityLocations.contains(playerInfo.lookingAtLocationWorld)) {

        Inventory* inventory;
        BlockEntityId id = blockEntityManager.blockEntityLocations.at(playerInfo.lookingAtLocationWorld);
        openBlockInventory = tryGetInventory(blockEntityManager.entities[id], inventory);

        /*std::cout << "looking at: " << playerInfo.lookingAtLocationWorld.x << " " <<
        playerInfo.lookingAtLocationWorld.y << " " << playerInfo.lookingAtLocationWorld.z << std::endl; std::cout <<
        "locations are:" << std::endl; for (auto& pair : blockEntityManager.blockEntityLocations) { std::cout << "
        " << pair.first.x << " " << pair.first.y << " " << pair.first.z << " type: " <<
        (int)blockEntityManager.entities[pair.second]->type << std::endl;
        }
        std::cout << "opening block inventory: " << openBlockInventory << " from block type: " <<
        (int)blockEntityManager.entities[id]->type << std::endl;*/

        playerInfo.interactedWithBlock = false;
    }

    if (inputHandler.keyPressed(GLFW_KEY_E) || openBlockInventory) {
        if (playerInfo.inventoryIsActive) {
            playerInfo.inventoryIsActive = false;
            inputHandler.disableCursor();
        }
        else {
            playerInfo.inventoryIsActive = true;
            inputHandler.enableCursor();
        }
    }

    if (playerInfo.inventoryIsActive) {
        updateActiveInventory(playerInfo, uiManager, blockEntityManager);
    }
}
