#pragma once

#include "Item.hpp"
#include <cstdint>

struct ItemStack
{
	Item item;
	uint32_t amount;
};