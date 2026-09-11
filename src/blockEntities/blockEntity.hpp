#pragma once

#include <cinttypes>

#include "Inventory.hpp"
#include "InventoryLayouts.hpp"

struct BlockEntityType {
    enum Value : uint32_t
    {
        none = 0,
        furnace = 1,
        drill = 2,
        pipe = 3,
        pipeIn = 4,
        pipeOut = 5,

        maxEnum = 6,
    };
    Value v;
    constexpr BlockEntityType() {}
    constexpr BlockEntityType(Value val) : v(val) {}
    constexpr operator uint32_t() const { return v; }
};

inline const InventoryLayout blockEntityToInventoryLayout[BlockEntityType::maxEnum] = {
    InventoryLayout::grid10x4Inventory,
    InventoryLayout::output1Input1,
    InventoryLayout::slot1,
};

struct BlockEntity
{
    BlockEntityType type;
};

struct BlockEntityFurnace
{
    BlockEntityType type = BlockEntityType::furnace;
    Inventory inventory = Inventory(2);
};

struct BlockEntityDrill
{
    BlockEntityType type = BlockEntityType::drill;
    Inventory inventory = Inventory(1);
};

struct BlockEntityPipe
{
    BlockEntityType type = BlockEntityType::pipe;
};
struct BlockEntityPipeIn
{
    BlockEntityType type = BlockEntityType::pipeIn;
};
struct BlockEntityPipeOut
{
    BlockEntityType type = BlockEntityType::pipeOut;
};

bool tryGetInventory(BlockEntity* entity, Inventory*& inventory);
