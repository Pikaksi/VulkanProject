#include "blockEntity.hpp"
#include "assertm.hpp"
#include <string>

bool tryGetInventory(BlockEntity* entity, Inventory*& inventory)
{
    BlockEntityType type = entity->type;
    if (type == BlockEntityType::furnace) {
        inventory = &((BlockEntityFurnace*)entity)->inventory;
        return true;
    }
    else if (type == BlockEntityType::drill) {
        inventory = &((BlockEntityDrill*)entity)->inventory;
        return true;
    }
    return false;
}
