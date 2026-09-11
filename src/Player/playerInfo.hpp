#pragma once

#include "Inventory/ItemStack.hpp"
#include "Inventory/InventoryLayouts.hpp"
#include "Inventory/Inventory.hpp"
#include "blockEntityManager.hpp"

struct PlayerInfo
{
    bool inventoryIsActive = false;
    int selectedHotbarSlot = 0;

    bool interactedWithBlock = false;
    bool isLookingAtBlock = false;
    glm::i32vec3 lookingAtLocationWorld = glm::ivec3{0, 0, 0};
    glm::i32vec3 lookingAtLocationChunk = glm::ivec3{0, 0, 0};
    glm::i32vec3 lookingAtLocationInChunk = glm::ivec3{0, 0, 0};

    const InventoryLayout playerInventoryLayout = InventoryLayout::grid10x4Inventory;
    Inventory playerInventory = Inventory(40);
    Inventory cursorInventory = Inventory(1);
};

