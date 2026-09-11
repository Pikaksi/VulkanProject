#pragma once

#include <unordered_map>
#include <vector>
#include <span>
#include "ItemStack.hpp"
#include "blockEntity.hpp"

struct ShapelessCraft
{
    std::vector<ItemStack> input;
    std::vector<ItemStack> output;
};

// clang-format off
inline const std::vector<ShapelessCraft> recipes[(size_t)BlockEntityType::maxEnum]{
    {}, // none
    { // furnace
        ShapelessCraft{
            .input = {ItemStack{.item = Item::oakLog, 1}},
            .output = {ItemStack{.item = Item::oakPlank, 1}},
        },
        ShapelessCraft{
            .input = {ItemStack{.item = Item::oakPlank, 1}},
            .output = {ItemStack{.item = Item::stick, 2}},
        },
    },
};
// clang-format on

bool findCraftingRecipe(BlockEntityType entityType, std::span<ItemStack> itemStacks, int& outRecipe);
bool tryCraftRecipe(BlockEntityType entityType, int recipeIndex, std::span<ItemStack> input, std::span<ItemStack> output);
void initCraftingRecipes();
