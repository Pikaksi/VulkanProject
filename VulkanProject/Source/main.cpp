#include <iostream>
#include <stdexcept>

#include "Application.hpp"

#include "ECS/EntityManager.hpp"

int main() {
    entityManager.addEntity(EntityArchetype::inventory);
    entityManager.addEntity(EntityArchetype::network);
    entityManager.addEntity(EntityArchetype::inventoryAndNetwork);

    Inventory& inventory = entityManager.entities[0].getComponent<Inventory>(inventoryComponentIndex);
    inventory.setSize(10);
    Inventory& inventory2 = entityManager.entities[2].getComponent<Inventory>(inventoryComponentIndex);
    inventory2.setSize(8);

    BlockNetwork& blockNetwork2 = entityManager.entities[1].getComponent<BlockNetwork>(blockNetworkComponentIndex);
    blockNetwork2.vec3 = {1, 2, 3};
    BlockNetwork& blockNetwork3 = entityManager.entities[1].getComponent<BlockNetwork>(blockNetworkComponentIndex);
    std::cout << blockNetwork3.vec3.x << "\n";
    BlockNetwork& blockNetwork4 = entityManager.entities[2].getComponent<BlockNetwork>(blockNetworkComponentIndex);
    blockNetwork4.vec3 = {4, 5, 6};

    BlockNetwork& blockNetwork5 = entityManager.entities[2].getComponent<BlockNetwork>(blockNetworkComponentIndex);
    std::cout << blockNetwork5.vec3.x << "\n";

    Inventory& inventory3 = entityManager.entities[0].getComponent<Inventory>(inventoryComponentIndex);
    std::cout << inventory3.getSize() << "\n";
    Inventory& inventory4 = entityManager.entities[2].getComponent<Inventory>(inventoryComponentIndex);
    std::cout << inventory4.getSize();

    return EXIT_SUCCESS;
    try {
        Application::getInstance().run();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}