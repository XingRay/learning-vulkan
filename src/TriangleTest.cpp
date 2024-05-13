//
// Created by leixing on 2024/5/12.
//

#include "TriangleTest.h"
#include "vulkan/vulkan.hpp"

// That way GLFW will include its own definitions and automatically load the Vulkan header with it.
#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <iostream>
#include <map>
#include "QueueFamilyIndices.h"


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

    // The first three parameters specify the width, height and title of the mWindow.
    // The fourth parameter allows you to optionally specify a monitor to open the mWindow on
    // and the last parameter is only relevant to OpenGL.
    mWindow = glfwCreateWindow(mWidth, mHeight, "triangle test", nullptr, nullptr);
}

void TriangleTest::initVulkan() {
    createInstance();
    setupDebugMessenger();
    pickPhysicalDevice();
    createLogicDevice();
}

void TriangleTest::mainLoop() {
    while (!glfwWindowShouldClose(mWindow)) {
        glfwPollEvents();
    }
}

void TriangleTest::cleanUp() {
    if (mEnableValidationLayer) {
        destroyDebugUtilsMessengerExt(nullptr);
    }

    mDevice.destroy();

    mInstance.destroy(nullptr);

    glfwDestroyWindow(mWindow);
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

    vk::DebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if (mEnableValidationLayer) {
        if (!checkValidationLayerSupported()) {
            throw std::runtime_error("validation layers required, but not available !");
        }
        createInfo.setPpEnabledLayerNames(mValidationLayers.data());

        populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = &debugCreateInfo;
    } else {
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
    }


    vk::Result result = vk::createInstance(&createInfo, nullptr, &mInstance);
    if (result != vk::Result::eSuccess) {
        throw std::runtime_error("failed to create mInstance");
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

//    for (const char *layerName: mValidationLayers) {
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
    return std::all_of(mValidationLayers.begin(), mValidationLayers.end(), [&availableLayers](const char *layerName) {
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
    if (mEnableValidationLayer) {
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

void TriangleTest::destroyDebugUtilsMessengerExt(const vk::AllocationCallbacks *pAllocator) {
    DestroyDebugUtilsMessengerEXT(mInstance, mDebugMessenger, pAllocator);
}

void TriangleTest::setupDebugMessenger() {
    if (!mEnableValidationLayer) {
        return;
    }

    vk::DebugUtilsMessengerCreateInfoEXT createInfo;
    populateDebugMessengerCreateInfo(createInfo);

    if (CreateDebugUtilsMessengerEXT(mInstance, &createInfo, nullptr, &mDebugMessenger) != vk::Result::eSuccess) {
        throw std::runtime_error("failed to set up debug messenger!");
    }
}

void TriangleTest::pickPhysicalDevice() {
    auto devices = mInstance.enumeratePhysicalDevices();
    for (const auto &device: devices) {
        vk::PhysicalDeviceProperties properties = device.getProperties();
        std::cout << "\tdeviceName: " << properties.deviceName << "\tdeviceType: " << to_string(properties.deviceType) << std::endl;
    }

    // 简单查找需要的设备
    for (const auto &device: devices) {
        if (isDeviceSuitable(device)) {
            mPhysicalDevice = device;
            break;
        }
    }
    if (mPhysicalDevice == VK_NULL_HANDLE) {
        throw std::runtime_error("failed to find GPUs with vulkan support !");
    }

    // 计算每张卡的分数, 取最高分
//    std::multimap<int, vk::PhysicalDevice> candidates;
//    for (const auto &device: devices) {
//        int score = rateDeviceSuitability(device);
//        candidates.insert(std::make_pair(score, device));
//    }
//    if (candidates.rbegin()->first > 0) {
//        mPhysicalDevice = candidates.rbegin()->second;
//    } else {
//        throw std::runtime_error("failed to find a suitable GPU!");
//    }

    auto extensionProperties = mPhysicalDevice.enumerateDeviceExtensionProperties();
    for (const auto &extensionProperty: extensionProperties) {
        std::cout << "\textensionName: " << extensionProperty.extensionName << "\tspecVersion: " << extensionProperty.specVersion << std::endl;
    }
}

bool TriangleTest::isDeviceSuitable(vk::PhysicalDevice device) {
//    vkGetPhysicalDeviceProperties(device, &deviceProperties);
//    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
    vk::PhysicalDeviceProperties deviceProperties = device.getProperties();
    vk::PhysicalDeviceFeatures deviceFeatures = device.getFeatures();

    QueueFamilyIndices indices = findQueueFamilies(device);
    if (!indices.isComplete()) {
        return false;
    }
    std::cout << "deviceType: " << to_string(deviceProperties.deviceType) << std::endl;

//     核显
//    vk::PhysicalDeviceType::eIntegratedGpu;

//    独显
//    vk::PhysicalDeviceType::eDiscreteGpu;
    return deviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu
           && deviceFeatures.geometryShader;
}

int TriangleTest::rateDeviceSuitability(vk::PhysicalDevice device) {
    int score = 0;
//    vkGetPhysicalDeviceProperties(device, &deviceProperties);
//    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
    vk::PhysicalDeviceProperties deviceProperties = device.getProperties();
    vk::PhysicalDeviceFeatures deviceFeatures = device.getFeatures();

    if (deviceProperties.deviceType == vk::PhysicalDeviceType::eIntegratedGpu) {
        score += 1000;
    }

    score += (int) deviceProperties.limits.maxImageDimension2D;

    // application can not function without geometry shaders
    if (!deviceFeatures.geometryShader) {
        return 0;
    }

    return score;
}

void TriangleTest::createLogicDevice() {
    QueueFamilyIndices indices = findQueueFamilies(mPhysicalDevice);
    uint32_t queueFamilyIndex = indices.queueFamily.value();

    vk::DeviceQueueCreateInfo queueCreateInfo;
    queueCreateInfo.sType = vk::StructureType::eDeviceQueueCreateInfo;
    queueCreateInfo.queueFamilyIndex = queueFamilyIndex;
    queueCreateInfo.queueCount = 1;

    float queuePriority = 1.0f;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    vk::PhysicalDeviceFeatures deviceFeatures{};

    vk::DeviceCreateInfo deviceCreateInfo;
    deviceCreateInfo.sType = vk::StructureType::eDeviceCreateInfo;
    deviceCreateInfo.setPQueueCreateInfos(&queueCreateInfo);
    deviceCreateInfo.setQueueCreateInfoCount(1);
    deviceCreateInfo.pEnabledFeatures = &deviceFeatures;

    deviceCreateInfo.enabledExtensionCount = 0;
    if (mEnableValidationLayer) {
        deviceCreateInfo.enabledLayerCount = mValidationLayers.size();
        deviceCreateInfo.setPpEnabledLayerNames(mValidationLayers.data());
    } else {
        deviceCreateInfo.enabledLayerCount = 0;
    }

    // 创建逻辑设备的同时会根据 deviceCreateInfo.pQueueCreateInfo 创建任务队列
    mDevice = mPhysicalDevice.createDevice(deviceCreateInfo);
    // 从逻辑设备中取出任务队列, 第二个参数为下标, 总共创建了一个队列,所以这里下标为 0
    mGraphicQueue = mDevice.getQueue(queueFamilyIndex, 0);
}

QueueFamilyIndices TriangleTest::findQueueFamilies(vk::PhysicalDevice &device) {
    QueueFamilyIndices indices;
    auto queueFamilyProperties = device.getQueueFamilyProperties();

    int i = 0;
    for (const auto &queueFamilyProperty: queueFamilyProperties) {
        if (queueFamilyProperty.queueFlags & vk::QueueFlagBits::eGraphics) {
            indices.queueFamily = i;
        }
        if (indices.isComplete()) {
            break;
        }
        i++;
    }

    return indices;
}

vk::Result CreateDebugUtilsMessengerEXT(vk::Instance instance,
                                        const vk::DebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                        const vk::AllocationCallbacks *pAllocator,
                                        vk::DebugUtilsMessengerEXT *pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func == nullptr) {
        return vk::Result::eErrorExtensionNotPresent;
    }

    VkResult vkResult = func(instance, (VkDebugUtilsMessengerCreateInfoEXT *) pCreateInfo, (VkAllocationCallbacks *) pAllocator, (VkDebugUtilsMessengerEXT *) pDebugMessenger);
    if (vkResult != VK_SUCCESS) {
        throw std::runtime_error("vkCreateDebugUtilsMessengerEXT failed !");
    }
    return vk::Result::eSuccess;
}

void DestroyDebugUtilsMessengerEXT(vk::Instance instance, vk::DebugUtilsMessengerEXT debugMessenger, const vk::AllocationCallbacks *pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func == nullptr) {
        throw std::runtime_error("func vkDestroyDebugUtilsMessengerEXT not found!");
    }

    func(instance, debugMessenger, (VkAllocationCallbacks *) pAllocator);
}

void populateDebugMessengerCreateInfo(vk::DebugUtilsMessengerCreateInfoEXT &createInfo) {
    createInfo.sType = vk::StructureType::eDebugUtilsMessengerCreateInfoEXT;
    createInfo.messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo
                                 | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning
                                 | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError;
    createInfo.messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral
                             | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation
                             | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance
                             | vk::DebugUtilsMessageTypeFlagBitsEXT::eDeviceAddressBinding;
    createInfo.pfnUserCallback = debugCallback;
}