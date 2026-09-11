#pragma once

#include <map>

#include "Inventory/Item.hpp"
#include "UITexLayer.hpp"

static const std::map<Item, UITexLayer> itemToUITexLayer = {
    {Item::dirt, UITexLayer::itemDirt},
    {Item::grassBlock, UITexLayer::itemGrassBlock},
    {Item::oakLog, UITexLayer::itemOakLog},
    {Item::stick, UITexLayer::itemStick},
    {Item::stone, UITexLayer::itemStone},
    {Item::oakPlank, UITexLayer::itemOakPlank},
    {Item::furnaceBlock, UITexLayer::itemFurnace},
    {Item::drillBlock, UITexLayer::itemDrillBlock},
    {Item::pipeBlock, UITexLayer::itemPipeBlock},
    {Item::pipeInBlock, UITexLayer::itemPipeInBlock},
    {Item::pipeOutBlock, UITexLayer::itemPipeOutBlock},
};
