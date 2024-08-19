#pragma once

#include <vector>

#include "Inventory/Inventory.hpp"

enum class EntityArchetype
{
    inventory = 0,
    inventoryAndConnector = 1
};

std::vector<Inventory> inventoryArchetype;