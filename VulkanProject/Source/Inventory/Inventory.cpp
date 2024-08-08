#include "Inventory.hpp"

#include <stdexcept>

#include "Constants.hpp"

bool moveItemFromInventory(int itemSlot, Inventory srcInventory, Inventory dstInventory)
{
    ItemStack itemStackToMove = srcInventory.getItem(itemSlot);

    if (itemStackToMove.item == Item::empty) {
        return true;
    }

    if (dstInventory.hasSpaceForItem(itemStackToMove)) {
        dstInventory.insertItem(itemStackToMove);
        srcInventory.setItem(itemSlot, ItemStack(Item::empty, 0));
        return true;
    }
    return false;
}

void swapSlots(int firstSlot, int secondSlot, Inventory firstInventory, Inventory secondInventory)
{
    ItemStack temporaryItemStack = firstInventory.getItem(firstSlot);
    firstInventory.setItem(firstSlot, secondInventory.getItem(secondSlot));
    secondInventory.setItem(secondSlot, temporaryItemStack);
}

void Inventory::insertItem(ItemStack itemStack)
{
#ifndef NDEBUG
    if (!hasSpaceForItem(itemStack)) {
        throw std::runtime_error("tried to insert item but the inventory did not have space for the item");
    }
#endif

    for (int i = 0; i < itemStacks.size(); i++) {
        if (itemStacks[i].item == itemStack.item) {
            uint32_t itemsToAdd = std::min(ITEM_STACK_MAX_SIZE - itemStacks[i].amount, itemStack.amount);

            itemStacks[i].amount += itemsToAdd;
            itemStack.amount -= itemsToAdd;

            if (itemStack.amount == 0) {
                return;
            }
        }
    }

    for (int i = 0; i < itemStacks.size(); i++) {
        if (itemStacks[i].item == Item::empty) {
            itemStacks[i] = itemStack;
            return;
        }
    }
}

void Inventory::removeItem(ItemStack itemStack)
{
#ifndef NDEBUG
    if (!hasItem(itemStack)) {
        throw std::runtime_error("Tried to delete item but it did not exist.");
    }
#endif

    for (int i = 0; i < itemStacks.size(); i++) {
        if (itemStacks[i].item == itemStack.item) {
            uint32_t itemsToRemove = std::min(itemStacks[i].amount, itemStack.amount);

            itemStacks[i].amount -= itemsToRemove;
            itemStack.amount -= itemsToRemove;

            if (itemStack.amount == 0) {
                break;
            }
        }
    }
}

bool Inventory::hasItem(ItemStack itemStack)
{
    int itemCount = 0;

    for (int i = 0; i < itemStacks.size(); i++) {
        if (itemStacks[i].item == itemStack.item) {
            itemCount += itemStacks[i].amount;
        }
    }

    if (itemStack.amount <= itemCount) {
        return true;
    }
    return false;
}

bool Inventory::hasSpaceForItem(ItemStack itemStack)
{
    for (int i = 0; i < itemStacks.size(); i++) {
        if (itemStacks[i].item == Item::empty) {
            return true;
        }
    }

    uint32_t availableSpace = 0;

    for (int i = 0; i < itemStacks.size(); i++) {
        if (itemStacks[i].item == itemStack.item) {
            availableSpace += ITEM_STACK_MAX_SIZE - itemStacks[i].amount;
        }
    }
    if (availableSpace >= itemStack.amount) {
        return true;
    }
    return false;
}

void Inventory::swapSlots(int firstSlot, int secondSlot)
{
    ItemStack temporaryItemStack = itemStacks[firstSlot];
    itemStacks[firstSlot] = itemStacks[secondSlot];
    itemStacks[secondSlot] = temporaryItemStack;
}

ItemStack Inventory::getItem(int itemSlot)
{
    return itemStacks[itemSlot];
}


void Inventory::setItem(int itemSlot, ItemStack itemStack)
{
    itemStacks[itemSlot] = itemStack;
}

uint32_t Inventory::getSize()
{
    return itemStacks.size();
}