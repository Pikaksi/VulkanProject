#pragma once

#include <map>

#include "Inventory/Item.hpp"
#include "UITexLayer.hpp"

std::map<Item, UITexLayer> itemToUITexLayer = {
    {Item::dirt, UITexLayer::itemDirt}, 
    {Item::grassBlock, UITexLayer::itemGrassBlock}, 
    {Item::oakLog, UITexLayer::itemOakLog}, 
    {Item::oakPlanks, UITexLayer::white}, 
    {Item::stick, UITexLayer::itemStick}, 
    {Item::stone, UITexLayer::itemStone}
};