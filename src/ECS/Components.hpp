#pragma once

#include <glm/vec3.hpp>

#include <cstdint>
#include <bit>

#include "Inventory/Inventory.hpp"

template <typename T>
struct ComponentTypeMap
{ 
    static const int value;
};

const int inventoryComponentIndex = 0;
const uint64_t inventoryComponentBitmask = 0b1 << inventoryComponentIndex;

template<typename T>
constexpr int getComponentIndex()
{
    return inventoryComponentIndex;
}

constexpr int getComponentIndex(uint64_t componentBitmask)
{
    return std::countr_zero(componentBitmask);
}
