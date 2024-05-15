#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <iostream>
#include "vulkan/vulkan.hpp"

#include "TriangleTest.h"
#include <stdexcept>
#include <cstdlib>

int main() {
//    glfwInit();
//
//    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
//    GLFWwindow *mWindow = glfwCreateWindow(800, 600, "Vulkan mWindow", nullptr, nullptr);
//
//    uint32_t extensionCount = 0;
//    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
//
//    std::cout << extensionCount << " extensions supported\n";
//
//    glm::mat4 matrix;
//    glm::vec4 vec;
//    auto test = matrix * vec;
//    printf("test: %f", test[0]);
//
//    while (!glfwWindowShouldClose(mWindow)) {
//        glfwPollEvents();
//    }
//
//    glfwDestroyWindow(mWindow);
//
//    glfwTerminate();

    TriangleTest test;

    try {
        test.run();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}