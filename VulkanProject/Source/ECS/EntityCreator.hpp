#pragma once

#include <glm/vec3.hpp>

#include <vector>

#include "Inventory/Inventory.hpp"

enum class EntityArchetype
{
    inventory = 0,
    inventoryAndConnector = 1
};

uint32_t createInventoryBlockEntity(glm::ivec3 blockLocation);