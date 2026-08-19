#include "blockEntity.hpp"
#include "assertm.hpp"
#include <string>

bool tryGetInventory(BlockEntity* entity, Inventory& inventory)
{
    BlockEntityType type = entity->type;
    if (type == BlockEntityType::furnace) {
        inventory = ((BlockEntityFurnace*)entity)->inventory;
        return true;
    }
    else {
        assertm(false, "tryGetInventory has not been implemented for type " << (int)type);
    }
    return false;
}
