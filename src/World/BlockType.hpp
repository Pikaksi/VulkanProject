#pragma once

#include <stdint.h>

enum BlockType : int8_t
{
	air = 0,
	stone = 1,
	grass = 2,
	dirt = 3,
	oakLog = 4,
	oakLeaf = 5,
	grassPlant = 6,
	furnace = 7,
	maxEnum = 8
};
