//
// Created by leixing on 2024/5/12.
//

#include "TriangleTest.h"
#include "vulkan/vulkan.hpp"

// That way GLFW will include its own definitions and automatically load the Vulkan header with it.
#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <iostream>


TriangleTest::TriangleTest() {

}

TriangleTest::~TriangleTest() {

}

void TriangleTest::run() {
    initWindow();
    initVulkan();
    mainLoop();
    cleanUp();
}

void TriangleTest::initWindow() {
    glfwInit();

    // Because GLFW was originally designed to create an OpenGL context,
    // we need to tell it to not create an OpenGL context with a subsequent call:
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    // Because handling resized windows takes special care that we'll look into later, disable it for now
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    // The first three parameters specify the width, height and title of the window.
    // The fourth parameter allows you to optionally specify a monitor to open the window on
    // and the last parameter is only relevant to OpenGL.
    window = glfwCreateWindow(WIDTH, HEIGHT, "triangle test", nullptr, nullptr);
}

void TriangleTest::initVulkan() {
    createInstance();
    setupDebugMessenger();
}

void TriangleTest::mainLoop() {
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }
}

void TriangleTest::cleanUp() {
    if (enableValidationLayer) {
        destroyDebugUtilsMessengerExt(nullptr);
    }

    instance.destroy(nullptr);

    glfwDestroyWindow(window);
    glfwTerminate();
}

void TriangleTest::createInstance() {
    vk::ApplicationInfo appInfo;
    appInfo.sType = vk::StructureType::eApplicationInfo;
    appInfo.setPApplicationName("triangle test");
    appInfo.setApplicationVersion(VK_MAKE_VERSION(1, 0, 0));
    appInfo.setPEngineName("no engine");
    appInfo.setEngineVersion(VK_MAKE_VERSION(1, 0, 0));
    appInfo.setApiVersion(VK_API_VERSION_1_3);

    vk::InstanceCreateInfo createInfo;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.sType = vk::StructureType::eInstanceCreateInfo;

    std::vector<const char *> extensions = getRequiredExtensions();
    createInfo.enabledExtensionCount = extensions.size();
    createInfo.setPpEnabledExtensionNames(extensions.data());
    std::cout << "glfw required extensions \n";
    for (auto &extension: extensions) {
        std::cout << '\t' << extension << std::endl;
    }

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if (enableValidationLayer) {
        if (!checkValidationLayerSupported()) {
            throw std::runtime_error("validation layers required, but not available !");
        }
        createInfo.setPpEnabledLayerNames(validationLayers.data());

        populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = &debugCreateInfo;
    } else {
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
    }


    vk::Result result = vk::createInstance(&createInfo, nullptr, &instance);
    if (result != vk::Result::eSuccess) {
        throw std::runtime_error("failed to create instance");
    }
    std::cout << "createInstance success !\n";

    // 展示所有的 vk 扩展
    uint32_t allExtensionCount = 0;
    result = vk::enumerateInstanceExtensionProperties(nullptr, &allExtensionCount, nullptr);
    if (result != vk::Result::eSuccess) {
        throw std::runtime_error("failed to get extension count");
    }
    std::vector<vk::ExtensionProperties> allExtensions(allExtensionCount);
    result = vk::enumerateInstanceExtensionProperties(nullptr, &allExtensionCount, allExtensions.data());
    if (result != vk::Result::eSuccess) {
        throw std::runtime_error("failed to get extensions");
    }

    std::cout << "available extensions \n";
    for (const auto &extension: allExtensions) {
        std::cout << '\t' << extension.extensionName << '\n';
    }
}

bool TriangleTest::checkValidationLayerSupported() {
    uint32_t availableLayerCount;
    vk::Result result = vk::enumerateInstanceLayerProperties(&availableLayerCount, nullptr);
    if (result != vk::Result::eSuccess) {
        throw std::runtime_error("get validation availableLayers failed !");
    }

    std::vector<vk::LayerProperties> availableLayers(availableLayerCount);
    result = vk::enumerateInstanceLayerProperties(&availableLayerCount, availableLayers.data());
    if (result != vk::Result::eSuccess) {
        throw std::runtime_error("get validation availableLayers failed !");
    }

//    for (const char *layerName: validationLayers) {
//        bool found = false;
//        for (const auto &layerProperty: availableLayers) {
//            if (strcmp(layerName, layerProperty.layerName) == 0) {
//                found = true;
//                break;
//            }
//        }
//
//        if (!found) {
//            return false;
//        }
//    }
//
//    return true;
    return std::all_of(validationLayers.begin(), validationLayers.end(), [&availableLayers](const char *layerName) {
        return std::find_if(availableLayers.begin(), availableLayers.end(), [layerName](const vk::LayerProperties &property) {
            return strcmp(property.layerName, layerName) == 0;
        }) != availableLayers.end();
    });
}

std::vector<const char *> TriangleTest::getRequiredExtensions() {
    uint32_t glfwRequiredExtensionCount = 0;
    // 查询 glfw 需要的扩展
    const char **glfwRequiredExtensions = glfwGetRequiredInstanceExtensions(&glfwRequiredExtensionCount);

    std::vector<const char *> extensions(glfwRequiredExtensions, glfwRequiredExtensions + glfwRequiredExtensionCount);
    if (enableValidationLayer) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
        void *pUserData) {

    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}

void TriangleTest::destroyDebugUtilsMessengerExt(const VkAllocationCallbacks *pAllocator) {
    DestroyDebugUtilsMessengerEXT(instance, debugMessenger, pAllocator);
}

void TriangleTest::setupDebugMessenger() {
    if (!enableValidationLayer) {
        return;
    }

    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    populateDebugMessengerCreateInfo(createInfo);

    if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
        throw std::runtime_error("failed to set up debug messenger!");
    }
}

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func == nullptr) {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }

    return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks *pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func == nullptr) {
        throw std::runtime_error("func vkDestroyDebugUtilsMessengerEXT not found!");
    }

    func(instance, debugMessenger, pAllocator);
}

void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo) {
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
                                 | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
                                 | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
                             | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
                             | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
}