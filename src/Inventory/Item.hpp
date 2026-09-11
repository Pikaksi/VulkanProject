#pragma once

#include <cinttypes>

enum class Item : uint32_t
{
    empty = 0,
    stone = 1,
    dirt = 2,
    grassBlock = 3,
    oakLog = 4,
    stick = 5,
    oakPlank = 6,
    furnaceBlock = 7,
    drillBlock = 8,
    pipeInBlock = 9,
    pipeOutBlock = 10,
    pipeBlock = 11,

    maxEnum = 12,
};
