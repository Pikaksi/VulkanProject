#pragma once

#include "Item.hpp"
#include <cstdint>

struct ItemSlot
{
	Item item;
	uint32_t amount;
};