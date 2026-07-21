#include <iostream>
#include <stdexcept>

#include "Application.hpp"

#include "util/tlsfAllocator.hpp"

int main()
{
    try {
        Application::getInstance().run();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
