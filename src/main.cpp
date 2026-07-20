#include <iostream>
#include <stdexcept>

#include "Application.hpp"

#include "ECS/EntityManager.hpp"

int main() {
    /*entityManager.addEntity(inventoryComponentBitmask);
    entityManager.addEntity(blockNetworkComponentBitmask);
    entityManager.addEntity(inventoryComponentBitmask | blockNetworkComponentBitmask);

    Inventory& inventory0 = entityManager.entities[0].getComponent<Inventory>();
    inventory0.setSize(10);
    Inventory& inventory2 = entityManager.entities[2].getComponent<Inventory>();
    inventory2.setSize(8);

    BlockNetwork& blockNetwork1 = entityManager.entities[1].getComponent<BlockNetwork>();
    blockNetwork1.vec3 = {1, 2, 3};
    BlockNetwork& blockNetwork2 = entityManager.entities[2].getComponent<BlockNetwork>();
    blockNetwork2.vec3 = {4, 5, 6};

    entityManager.deleteEntity(1);

    entityManager.addEntity(inventoryComponentBitmask);

    Inventory& inventory3 = entityManager.entities[1].getComponent<Inventory>();
    inventory3.setSize(60);

    Inventory& inventory01 = entityManager.entities[0].getComponent<Inventory>();
    std::cout << inventory01.getSize() << "\n";
    Inventory& inventory21 = entityManager.entities[2].getComponent<Inventory>();
    std::cout << inventory21.getSize() << "\n";

    BlockNetwork& blockNetwork21 = entityManager.entities[2].getComponent<BlockNetwork>();
    std::cout << blockNetwork21.vec3.x << "\n";


    Inventory& blockNetwork31 = entityManager.entities[1].getComponent<Inventory>();
    std::cout << blockNetwork31.getSize() << "\n";*/

    try {
        Application::getInstance().run();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}