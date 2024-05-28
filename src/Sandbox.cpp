#include <iostream>
#include <stdexcept>

#include "TriangleTest.h"

int main() {
    TriangleTest test;

    try {
        test.run();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}