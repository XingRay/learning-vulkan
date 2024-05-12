//
// Created by leixing on 2024/5/12.
//

#pragma once

#include <cstdint>
#include <GLFW/glfw3.h>
#include "vulkan/vulkan.hpp"

class TriangleTest {
public:
    // public fields
private:
    // private fields
    const int32_t WIDTH = 800;
    const int32_t HEIGHT = 600;

    const std::vector<const char*> validationLayers = {
            "VK_LAYER_KHRONOS_validation"
    };

#ifdef NDEBUG
    const bool enableValidationLayer = false;
#else
    const bool enableValidationLayer = true;
#endif

    GLFWwindow* window;

    vk::Instance instance;

    VkDebugUtilsMessengerEXT debugMessenger;


public:
    TriangleTest();
    ~TriangleTest();
    void run();

private:

    void initWindow();

    void initVulkan();

    void mainLoop();

    void cleanUp();

    void createInstance();

    bool checkValidationLayerSupported();

    std::vector<const char*> getRequiredExtensions();

    void setupDebugMessenger();

//    VkResult createDebugUtilsMessengerEXT(VkInstance instance,
//                                          const VkDebugUtilsMessengerCreateInfoEXT *createInfo,
//                                          const VkAllocationCallbacks *allocator,
//                                          VkDebugUtilsMessengerEXT *callback);

    void destroyDebugUtilsMessengerExt(const VkAllocationCallbacks *pAllocator);
};


VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pDebugMessenger);
void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);