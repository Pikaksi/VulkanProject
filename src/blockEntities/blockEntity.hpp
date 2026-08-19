#pragma once

#include <cinttypes>

#include "Inventory.hpp"

enum class BlockEntityType : uint32_t
{
    none = 0,
    furnace = 1,
};

struct BlockEntity
{
    BlockEntityType type;
};

struct BlockEntityFurnace
{
    BlockEntityType type;
    Inventory inventory;
};

bool tryGetInventory(BlockEntity* entity);
