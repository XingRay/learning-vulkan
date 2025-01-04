//
// Created by leixing on 2024/12/22.
//

#include "VulkanInstance.h"
#include "VkCheck.h"
#include "VkCheckCpp.h"
#include "Log.h"

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

namespace engine {

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
            void *pUserData);

    VulkanInstance::VulkanInstance(const std::vector<const char *> &requiredInstanceExtensions,
                                   const std::vector<const char *> &layers) {
        LOG_D("VulkanInstance::VulkanInstance");
        vk::DynamicLoader dl;

        auto vkGetInstanceProcAddr = dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
        if (vkGetInstanceProcAddr == nullptr) {
            throw std::runtime_error("Failed to load vkGetInstanceProcAddr");
        }

        auto vkEnumerateInstanceExtensionProperties = dl.getProcAddress<PFN_vkEnumerateInstanceExtensionProperties>("vkEnumerateInstanceExtensionProperties");
        if (vkEnumerateInstanceExtensionProperties == nullptr) {
            throw std::runtime_error("Failed to load vkEnumerateInstanceExtensionProperties");
        }

        auto vkEnumerateInstanceLayerProperties = dl.getProcAddress<PFN_vkEnumerateInstanceLayerProperties>("vkEnumerateInstanceLayerProperties");
        if (vkEnumerateInstanceLayerProperties == nullptr) {
            throw std::runtime_error("Failed to load vkEnumerateInstanceLayerProperties");
        }

        auto vkCreateInstance = dl.getProcAddress<PFN_vkCreateInstance>("vkCreateInstance");
        if (vkCreateInstance == nullptr) {
            throw std::runtime_error("Failed to load vkCreateInstance");
        }

        uint32_t extensionCount = 0;
        CALL_VK(vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr));
        std::vector<VkExtensionProperties> availableInstanceExtensions(extensionCount);
        CALL_VK(vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, availableInstanceExtensions.data()));

        LOG_D("Available instance extensions:[%d]", extensionCount);
        for (const auto &extensionProperties: availableInstanceExtensions) {
            LOG_D("  %s", extensionProperties.extensionName);
        }

        for (const auto &instanceExtension: requiredInstanceExtensions) {
            bool found = false;
            for (const auto &available: availableInstanceExtensions) {
                if (strcmp(instanceExtension, available.extensionName) == 0) {
                    found = true;
                    break;
                }
            }
            if (found) {
                mEnabledInstanceExtensionNames.push_back(instanceExtension);
            } else {
                LOG_W("Required extension %s is not available", instanceExtension);
            }
        }

        LOG_D("Enabled extensions:[%lld]", mEnabledInstanceExtensionNames.size());
        for (const auto &extensionName: mEnabledInstanceExtensionNames) {
            LOG_D("  %s", extensionName);
        }

        uint32_t layerCount = 0;
        CALL_VK(vkEnumerateInstanceLayerProperties(&layerCount, nullptr));
        std::vector<VkLayerProperties> availableLayers(layerCount);
        CALL_VK(vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data()));
        LOG_D("Available layers:[%d]", layerCount);
        for (const auto &layerProperties: availableLayers) {
            LOG_D("  %s", layerProperties.layerName);
        }

        for (const auto &layer: layers) {
            bool found = false;
            for (const auto &available: availableLayers) {
                if (strcmp(layer, available.layerName) == 0) {
                    found = true;
                    break;
                }
            }
            if (found) {
                mEnabledLayerNames.push_back(layer);
            } else {
                LOG_W("Required layer %s is not available", layer);
            }
        }

        LOG_D("Enabled layers:[%lld]", mEnabledLayerNames.size());
        for (const auto &layerName: mEnabledLayerNames) {
            LOG_D("  %s", layerName);
        }


        VkApplicationInfo appInfo = {
                .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
                .pNext = nullptr,
                .pApplicationName = "vulkan_application",
                .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
                .pEngineName = "vulkan_engine",
                .engineVersion = VK_MAKE_VERSION(1, 0, 0),
                .apiVersion = VK_API_VERSION_1_3,
        };

        VkInstanceCreateInfo instanceCreateInfo{
                .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
                .pNext = nullptr,
                .pApplicationInfo = &appInfo,
                .enabledLayerCount = static_cast<uint32_t>(mEnabledLayerNames.size()),
                .ppEnabledLayerNames = mEnabledLayerNames.data(),
                .enabledExtensionCount = static_cast<uint32_t>(mEnabledInstanceExtensionNames.size()),
                .ppEnabledExtensionNames = mEnabledInstanceExtensionNames.data(),
        };

        vk::DebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
//        if (mEnableValidationLayer) {
//            if (!checkValidationLayerSupported()) {
//                throw std::runtime_error("validation layers required, but not available !");
//            }
//            createInfo.setPEnabledLayerNames(mValidationLayers);

        populateDebugMessengerCreateInfo(debugCreateInfo);
        instanceCreateInfo.pNext = &debugCreateInfo;
//        } else {
//            createInfo.enabledLayerCount = 0;
//            createInfo.pNext = nullptr;
//        }

        CALL_VK(vkCreateInstance(&instanceCreateInfo, nullptr, (VkInstance *) &mInstance));

        vk::DispatchLoaderDynamic dld(mInstance, vkGetInstanceProcAddr);
        dld.init(dl);
        vk::defaultDispatchLoaderDynamic = dld;

//        setupDebugCallback();
    }


    VulkanInstance::~VulkanInstance() {
        LOG_D("VulkanInstance::~VulkanInstance()");
        if (mInstance != nullptr) {
//            mInstance.destroyDebugReportCallbackEXT(mDebugReportCallback);
            mInstance.destroy();
            mInstance = nullptr;
        } else {
            LOG_W("instance is null");
        }
    }


    void VulkanInstance::setupDebugCallback() {
        vk::DebugReportCallbackCreateInfoEXT debugInfo{
                vk::DebugReportFlagBitsEXT::eError | vk::DebugReportFlagBitsEXT::eWarning,
                debugCallbackImpl,
                nullptr
        };

        mDebugReportCallback = mInstance.createDebugReportCallbackEXT(debugInfo);
    }


    VKAPI_ATTR VkBool32 VKAPI_CALL VulkanInstance::debugCallbackImpl(
            VkDebugReportFlagsEXT flags,
            VkDebugReportObjectTypeEXT objectType,
            uint64_t object,
            size_t location,
            int32_t messageCode,
            const char *pLayerPrefix,
            const char *pMessage,
            void *pUserData) {
        if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT) {
            LOG_E("[ERROR] %s: %s", pLayerPrefix, pMessage);
        } else if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT) {
            LOG_E("[WARNING] %s: %s", pLayerPrefix, pMessage);
        } else {
            LOG_E("[INFO] %s: %s", pLayerPrefix, pMessage);
        }
        return VK_FALSE;
    }


    void VulkanInstance::populateDebugMessengerCreateInfo(vk::DebugUtilsMessengerCreateInfoEXT &createInfo) {
        createInfo.messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eError
                                     | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning
                                     | vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo;

        createInfo.messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral
                                 | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation
                                 | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance
                                 | vk::DebugUtilsMessageTypeFlagBitsEXT::eDeviceAddressBinding;
        createInfo.pfnUserCallback = debugCallback;
    }

    void VulkanInstance::setupDebugMessenger() {
        LOG_D("setupDebugMessenger");
//        if (!mEnableValidationLayer) {
//            return;
//        }

        vk::DebugUtilsMessengerCreateInfoEXT createInfo;
        populateDebugMessengerCreateInfo(createInfo);

        if (CreateDebugUtilsMessengerEXT(mInstance, &createInfo, nullptr, &mDebugMessenger) != vk::Result::eSuccess) {
            throw std::runtime_error("failed to set up debug messenger!");
        }
    }


    vk::Result VulkanInstance::CreateDebugUtilsMessengerEXT(vk::Instance instance,
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

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
            void *pUserData) {

        LOG_E("validation layer: %s", pCallbackData->pMessage);

        return VK_FALSE;
    }
}
