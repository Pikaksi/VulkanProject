#include "blockEntityCrafting.hpp"

#include <algorithm>
#include <cassert>
#include <span>

#include "assertm.hpp"
#include "Constants.hpp"
#include "blockEntity.hpp"

struct VectorItemHash
{
    size_t operator()(const std::vector<Item>& vec) const
    {
        size_t seed = vec.size();

        for (const Item item : vec) {
            // A robust combining algorithm (used by the Boost library)
            // This prevents issues where [A, A] or [B, A] create hash collisions.
            seed ^= (size_t)item + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }
};

std::unordered_map<std::vector<Item>, uint32_t, VectorItemHash> recipeInputs[(size_t)BlockEntityType::maxEnum];

// Returns true if a recipe was found
bool findCraftingRecipe(BlockEntityType entityType, std::span<ItemStack> itemStacks, int& outRecipe)
{
    std::vector<Item> items;
    for (int i = 0; i < itemStacks.size(); i++) {
        if (itemStacks[i].item == Item::empty) continue;
        items.push_back(itemStacks[i].item);
    }
    sort(items.begin(), items.end());
    auto it = std::unique(items.begin(), items.end());
    items.erase(it, items.end());

    auto& entityRecipeInputs = recipeInputs[(size_t)entityType];
    if (entityRecipeInputs.contains(items)) {
        outRecipe = entityRecipeInputs.at(items);
        return true;
    }
    return false;
}

int findItemCount(Item item, std::span<ItemStack> items)
{
    int amount = 0;
    for (int i = 0; i < items.size(); i++) {
        if (items[i].item == item) {
            amount += items[i].amount;
        }
    }
    return amount;
}

bool hasSpaceForItems(std::span<const ItemStack> itemsToAdd, std::span<const ItemStack> items)
{
    // TODO: make inserting more smart
    int outputInventoryIndex = 0;
    for (int i = 0; i < itemsToAdd.size(); i++) {
        ItemStack itemToAdd = itemsToAdd[i];

        while (outputInventoryIndex < items.size()) {
            if (items[i].item == Item::empty) {
                break;
            }
            else if (itemsToAdd[i].item == items[i].item && items[i].amount < ITEM_STACK_MAX_SIZE) {
                itemToAdd.amount -= ITEM_STACK_MAX_SIZE - items[i].amount;
                if (itemToAdd.amount <= 0) {
                    break;
                }
            }

            outputInventoryIndex += 1;
        }
    }
    return outputInventoryIndex < items.size();
}

void removeItem(ItemStack item, std::span<ItemStack> items)
{
    for (int i = 0; i < items.size(); i++) {
        if (items[i].item == item.item) {
            int amount = std::min(item.amount, items[i].amount);
            items[i].amount -= amount;
            if (items[i].amount <= 0) {
                items[i].item = Item::empty;
            }
            item.amount -= amount;
        }
    }
    assertm(item.amount == 0, "Failed to remove items when called to do so");
}

void addItems(std::span<const ItemStack> itemsToAdd, std::span<ItemStack> items)
{
    // TODO: Add smarter insertion
    int outputInventoryIndex = 0;
    for (int i = 0; i < itemsToAdd.size(); i++) {
        ItemStack itemToAdd = itemsToAdd[i];

        while (outputInventoryIndex < items.size()) {
            if (items[i].item == Item::empty) {
                items[i] = itemToAdd;
                break;
            }
            else if (itemsToAdd[i].item == items[i].item && items[i].amount < ITEM_STACK_MAX_SIZE) {
                int amount = std::min(itemToAdd.amount, ITEM_STACK_MAX_SIZE - items[i].amount);
                itemToAdd.amount -= amount;
                items[i].amount += amount;
                if (itemToAdd.amount <= 0) {
                    break;
                }
            }

            // increment output index if we could not place item into slot
            outputInventoryIndex += 1;
        }
    }
    assertm(outputInventoryIndex < items.size(), "Failed to add items when crafting");
}

void removeItems(std::span<const ItemStack> itemsToRemove, std::span<ItemStack> items)
{
    for (int i = 0; i < itemsToRemove.size(); i++) {
        removeItem(itemsToRemove[i], items);
    }
}

// Returns wether craft was successfull
bool tryCraftRecipe(BlockEntityType entityType, int recipeIndex, std::span<ItemStack> input, std::span<ItemStack> output)
{
    const ShapelessCraft& recipe = recipes[(size_t)entityType][recipeIndex];

    for (int inputIndex = 0; inputIndex < input.size(); inputIndex++) {
        if (findItemCount(recipe.input[inputIndex].item, input) < recipe.input[inputIndex].amount) {
            return false;
        }
    }
    if (hasSpaceForItems((std::span<const ItemStack>)recipe.output, output)) {
        addItems((std::span<const ItemStack>)recipe.output, output);
        removeItems((std::span<const ItemStack>)recipe.input, input);
        return true;
    }
    return false;
}

void initCraftingRecipes()
{
    for (uint64_t entityType = 0; entityType < (uint64_t)BlockEntityType::maxEnum; entityType++) {

        for (int recipe = 0; recipe < recipes[entityType].size(); recipe++) {
            std::vector<Item> requiredItems;
            for (const ItemStack itemStack : recipes[entityType][recipe].input) {
                requiredItems.push_back(itemStack.item);
            }
            std::sort(requiredItems.begin(), requiredItems.end());

            recipeInputs[entityType].insert(std::make_pair(requiredItems, recipe));
        }
    }
}
