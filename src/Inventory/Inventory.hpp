#pragma once

#include <iostream>
#include <vector>

#include "ItemStack.hpp"

struct Inventory
{
    std::vector<ItemStack> itemStacks;

    Inventory(uint32_t size)
    {
        setSize(size);
    }
    Inventory() {}

    void setSize(uint32_t size)
    {
        itemStacks.resize(size);
    }

    void insertItem(ItemStack itemStack);
    void removeItem(ItemStack itemStack);
    bool hasItem(ItemStack itemStack);
    bool hasSpaceForItem(ItemStack itemStack);
    void swapSlots(int firstSlot, int secondSlot);
    uint32_t getSize();
};

// Returns true if the item was moved. If moved item is empty return true.
bool moveItemFromInventory(int itemSlot, Inventory& srcInventory, Inventory& dstInventory);
void swapSlots(int firstSlot, int secondSlot, Inventory& firstInventory, Inventory& secondInventory);
