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
    inventory = entityManager.entities[2].getComponent<Inventory>(inventoryComponentIndex);
    inventory.setSize(8);
    inventory = entityManager.entities[0].getComponent<Inventory>(inventoryComponentIndex);
    std::cout << inventory.getSize() << "\n";

    BlockNetwork& blockNetwork = entityManager.entities[1].getComponent<BlockNetwork>(blockNetworkComponentIndex);
    blockNetwork.vec3 = {1, 2, 3};
    blockNetwork = entityManager.entities[2].getComponent<BlockNetwork>(blockNetworkComponentIndex);
    blockNetwork.vec3 = {4, 5, 6};

    inventory = entityManager.entities[2].getComponent<Inventory>(inventoryComponentIndex);
    std::cout << inventory.getSize() << "\n";

    blockNetwork = entityManager.entities[1].getComponent<BlockNetwork>(blockNetworkComponentIndex);
    std::cout << blockNetwork.vec3.x << "\n";
    blockNetwork = entityManager.entities[2].getComponent<BlockNetwork>(blockNetworkComponentIndex);
    std::cout << blockNetwork.vec3.x << "\n";

    Inventory& inventory2 = entityManager.entities[0].getComponent<Inventory>(inventoryComponentIndex);
    std::cout << inventory2.getSize();

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