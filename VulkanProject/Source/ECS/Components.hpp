#pragma once

#include <glm/vec3.hpp>

#include <cstdint>
#include <bit>

#include "Inventory/Inventory.hpp"

struct BlockNetwork
{
    glm::vec3 vec3;
};

template <typename T>
struct ComponentTypeMap
{ 
    static const int value;
};

const int inventoryComponentIndex = 0;
template <>
const int ComponentTypeMap<Inventory>::value = inventoryComponentIndex;
const uint64_t inventoryComponentBitmask = 0b1 << inventoryComponentIndex;

const int blockNetworkComponentIndex = 1;
const uint64_t blockNetworkComponentBitmask = 0b1 << blockNetworkComponentIndex;
template <>
const int ComponentTypeMap<BlockNetwork>::value = blockNetworkComponentIndex;

template<typename T>
constexpr int getComponentIndex()
{
    return ComponentTypeMap<T>::value;
}

constexpr int getComponentIndex(uint64_t componentBitmask)
{
    return std::countr_zero(componentBitmask);
}