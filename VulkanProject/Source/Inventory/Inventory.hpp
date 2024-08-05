#pragma once

#include <vector>

#include "ItemStack.hpp"

class Inventory
{
public:
    Inventory(uint32_t size) {
        itemStacks.resize(size);
    }

    void insertItem(ItemStack itemStack);
    void removeItem(ItemStack itemStack);
    bool hasItem(ItemStack itemStack);
    bool hasSpaceForItem(ItemStack itemStack);
    void swapSlots(int firstSlot, int secondSlot);
    ItemStack getItem(int itemSlot);
    void setItem(int itemSlot, ItemStack itemStack);
    uint32_t getSize();

private:
    std::vector<ItemStack> itemStacks;
};

// Returns true if the item was moved. If moved item is empty return true.
bool moveItemFromInventory(int itemSlot, Inventory srcInventory, Inventory dstInventory);
void swapSlots(int firstSlot, int secondSlot, Inventory firstInventory, Inventory secondInventory);