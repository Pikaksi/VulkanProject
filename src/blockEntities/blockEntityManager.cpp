#include "blockEntityManager.hpp"

#include <unordered_set>
#include <queue>

#include "Inventory.hpp"
#include "assertm.hpp"
#include "blockEntity.hpp"
#include "blockEntityCrafting.hpp"
#include "vec3hash.hpp"

const glm::i32vec3 dirs[6] = {
    {-1, 0,  0 },
    {1,  0,  0 },
    {0,  -1, 0 },
    {0,  1,  0 },
    {0,  0,  -1},
    {0,  0,  1 },
};

BlockEntityId BlockEntityManager::createEntity(glm::i32vec3 loc, BlockEntityType type)
{
    BlockEntity* entity = nullptr;
    if (type == BlockEntityType::furnace) {
        entity = (BlockEntity*)new BlockEntityFurnace();
    }
    else if (type == BlockEntityType::drill) {
        entity = (BlockEntity*)new BlockEntityDrill();
    }
    else if (type == BlockEntityType::pipe) {
        entity = (BlockEntity*)new BlockEntityPipe();
    }
    else if (type == BlockEntityType::pipeIn) {
        entity = (BlockEntity*)new BlockEntityPipeIn();
    }
    else if (type == BlockEntityType::pipeOut) {
        entity = (BlockEntity*)new BlockEntityPipeOut();
    }
    else {
        assertm(false, "Creating entity of type " << type << " has not been defined yet");
    }
    // TODO: Reuse indices
    entities.push_back(entity);
    BlockEntityId id = entities.size() - 1;
    blockEntityLocations.insert(std::make_pair(loc, id));

    return id;
}

bool tryGetNeightboringInventory(BlockEntityManager& blockEntityManager, glm::i32vec3& loc, Inventory*& outputInventory)
{
    for (auto dir : dirs) {
        auto loc2 = loc + dir;
        if (blockEntityManager.blockEntityLocations.contains(loc2)) {
            BlockEntityId id = blockEntityManager.blockEntityLocations.at(loc2);
            BlockEntity* entity = blockEntityManager.entities[id];
            Inventory* inventory = nullptr;
            if (tryGetInventory(entity, inventory)) {
                outputInventory = inventory;
                return true;
            }
        }
    }
    return false;
}

bool findPipeOutputInventory(BlockEntityManager& blockEntityManager,
                             glm::i32vec3& startingLoc,
                             Inventory*& outputInventory)
{
    std::unordered_set<glm::i32vec3, Vec3LocalizedHash> visited;
    std::queue<glm::i32vec3> visit;
    visit.push(startingLoc);
    while (visit.size() != 0) {
        glm::i32vec3 loc = visit.front();
        visit.pop();
        if (visited.contains(loc)) {
            continue;
        }

        visited.insert(loc);

        for (auto dir : dirs) {
            auto loc2 = loc + dir;
            if (blockEntityManager.blockEntityLocations.contains(loc2)) {
                BlockEntityId id = blockEntityManager.blockEntityLocations.at(loc2);
                BlockEntity* entity = blockEntityManager.entities[id];
                if (entity->type == BlockEntityType::pipe || entity->type == BlockEntityType::pipeIn) {
                    visit.push(loc2);
                }

                if (entity->type == BlockEntityType::pipeOut) {
                    if (tryGetNeightboringInventory(blockEntityManager, loc2, outputInventory)) {
                        return true;
                    }
                    visit.push(loc2);
                }
            }
        }
    }
    return false;
}

void updatePipeIn(BlockEntityManager& blockEntityManager, glm::i32vec3& loc)
{
    std::cout << "trying to find output inventory" << std::endl;
    for (auto dir : dirs) {
        auto loc2 = loc + dir;
        if (!blockEntityManager.blockEntityLocations.contains(loc2)) {
            continue;
        }

        BlockEntityId id = blockEntityManager.blockEntityLocations.at(loc2);
        BlockEntity* entity = blockEntityManager.entities[id];
        Inventory* inventory = nullptr;
        if (!tryGetInventory(entity, inventory)) {
            continue;
        }
        std::cout << "  found inventory to take from" << std::endl;
        if (inventory->itemStacks[0].item == Item::empty) {
            continue;
        }

        Inventory* outputInventory = nullptr;
        if (!findPipeOutputInventory(blockEntityManager, loc, outputInventory)) {
            continue;
        }
        std::cout << "  found output inventory" << std::endl;

        assertm(inventory != nullptr, "Inventory is null");
        assertm(outputInventory != nullptr, "Inventory is null");
        ItemStack itemsToMove = ItemStack{.item = inventory->itemStacks[0].item, .amount = 1};
        if (inventory->hasItem(itemsToMove) && outputInventory->hasSpaceForItem(itemsToMove)) {
            std::cout << "  moved items from: " << inventory << " to " << outputInventory << std::endl;
            inventory->removeItem(itemsToMove);
            outputInventory->insertItem(itemsToMove);
        }
    }
    std::cout << "  ended" << std::endl;
}

void BlockEntityManager::updateBlockEntity(BlockEntityId id, glm::i32vec3 loc)
{
    BlockEntity* entityGeneral = entities[id];

    if (entityGeneral->type == BlockEntityType::furnace) {
        BlockEntityFurnace* furnace = (BlockEntityFurnace*)entityGeneral;

        std::span<ItemStack> inputSlots(furnace->inventory.itemStacks.data(), 1);
        std::span<ItemStack> outputSlots(furnace->inventory.itemStacks.data() + 1, 1);

        int recipe = -1;
        bool foundRecipe = findCraftingRecipe(BlockEntityType::furnace, inputSlots, recipe);
        if (foundRecipe) {
            bool success = tryCraftRecipe(BlockEntityType::furnace, recipe, inputSlots, outputSlots);
        }
    }

    else if (entityGeneral->type == BlockEntityType::drill) {
        BlockEntityDrill* drill = (BlockEntityDrill*)entityGeneral;

        ItemStack itemsToAdd = ItemStack{.item = Item::stone, .amount = 1};
        if (drill->inventory.hasSpaceForItem(itemsToAdd)) {
            drill->inventory.insertItem(itemsToAdd);
        }
    }

    else if (entityGeneral->type == BlockEntityType::pipeIn) {
        updatePipeIn(*this, loc);
    }
}

void BlockEntityManager::updateBlockEntities()
{
    for (auto [loc, id] : blockEntityLocations) {
        updateBlockEntity(id, loc);
    }
}
