//
// Created by leixing on 2024/5/12.
//

#pragma once

#include <cstdint>
#include <GLFW/glfw3.h>
#include "vulkan/vulkan.hpp"
#include "QueueFamilyIndices.h"

class TriangleTest {
public:
    // public fields
private:
    // private fields
    const int32_t mWidth = 800;
    const int32_t mHeight = 600;

    const std::vector<const char*> mValidationLayers = {
            "VK_LAYER_KHRONOS_validation"
    };

#ifdef NDEBUG
    const bool mEnableValidationLayer = false;
#else
    const bool mEnableValidationLayer = true;
#endif

    GLFWwindow* mWindow;

    vk::Instance mInstance;

    vk::DebugUtilsMessengerEXT mDebugMessenger;

    // 物理设备
    vk::PhysicalDevice mPhysicalDevice;

    // 逻辑设备
    vk::Device mDevice;

    // 图形相关的任务队列
    vk::Queue mGraphicQueue;

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

    void destroyDebugUtilsMessengerExt(const vk::AllocationCallbacks *pAllocator);

    void pickPhysicalDevice();

    bool isDeviceSuitable(vk::PhysicalDevice device);

    int rateDeviceSuitability(vk::PhysicalDevice device);

    void createLogicDevice();

    QueueFamilyIndices findQueueFamilies(vk::PhysicalDevice& device);
};


vk::Result CreateDebugUtilsMessengerEXT(vk::Instance instance,
                                        const vk::DebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                        const vk::AllocationCallbacks *pAllocator,
                                        vk::DebugUtilsMessengerEXT *pDebugMessenger);

void DestroyDebugUtilsMessengerEXT(vk::Instance instance, vk::DebugUtilsMessengerEXT debugMessenger, const vk::AllocationCallbacks *pAllocator);

void populateDebugMessengerCreateInfo(vk::DebugUtilsMessengerCreateInfoEXT &createInfo);