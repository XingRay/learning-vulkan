//
// Created by leixing on 2024/5/12.
//

#pragma once

#include <cstdint>
#include <limits>
#include <algorithm>

// That way GLFW will include its own definitions and automatically load the Vulkan header with it.
#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>

#include "vulkan/vulkan.hpp"
#include "QueueFamilyIndices.h"
#include "SwapChainSupportDetail.h"



class TriangleTest {
public:
    // public fields
private:
    // private fields
    const int32_t mWidth = 800;
    const int32_t mHeight = 600;

    const std::vector<const char *> mValidationLayers = {
            "VK_LAYER_KHRONOS_validation"
    };

    const std::vector<const char *> mRequiredExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

#ifdef NDEBUG
    const bool mEnableValidationLayer = false;
#else
    const bool mEnableValidationLayer = true;
#endif

    GLFWwindow *mWindow;

    vk::Instance mInstance;

    vk::DebugUtilsMessengerEXT mDebugMessenger;

    // 物理设备
    vk::PhysicalDevice mPhysicalDevice;

    // 逻辑设备
    vk::Device mDevice;

    // 图形相关的任务队列
    vk::Queue mGraphicQueue;

    // 显示相关的任务队列
    vk::Queue mPresentQueue;

    vk::SurfaceKHR mSurface;

    vk::SwapchainKHR mSwapChain;

    vk::SurfaceFormatKHR mSwapChainImageFormat;

    vk::Extent2D mSwapChainExtent;

    std::vector<vk::Image> mSwapChainImages;

    std::vector<vk::ImageView> mSwapChainImageViews;

    vk::RenderPass mRenderPass;

    vk::PipelineLayout mPipelineLayout;

    vk::Pipeline mGraphicsPipeline;

    std::vector<vk::Framebuffer> mSwapChainFrameBuffers;

    vk::CommandPool mCommandPool;

    vk::CommandBuffer mCommandBuffer;

    vk::Semaphore mImageAvailableSemaphore;

    vk::Semaphore mRenderFinishedSemaphore;

    vk::Fence mInFlightFence;

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

    std::vector<const char *> getRequiredExtensions();

    void setupDebugMessenger();

    void destroyDebugUtilsMessengerExt(const vk::AllocationCallbacks *pAllocator);

    void pickPhysicalDevice();

    bool isDeviceSuitable(vk::PhysicalDevice device);

    int rateDeviceSuitability(vk::PhysicalDevice device);

    void createLogicDevice();

    QueueFamilyIndices findQueueFamilies(vk::PhysicalDevice &device);

    void createSurface();

    bool isDeviceSupportedRequiredExtensions(vk::PhysicalDevice device);

    SwapChainSupportDetail querySwapChainSupported(vk::PhysicalDevice& device);

    vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);

    vk::PresentModeKHR choosePresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes);

    vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capability);

    void createSwapChain();

    void createImageViews();

    void cleanImageViews();

    void createGraphicsPipeline();

    vk::ShaderModule createShaderModule(const std::vector<char>& code);

    void createRenderPass();

    void createFrameBuffers();

    void cleanFrameBuffers();

    void createCommandPool();

    void createCommandBuffer();

    void recordCommandBuffer(uint32_t imageIndex);

    void drawFrame();

    void createSyncObjects();

    void cleanSyncObjects();
};


vk::Result CreateDebugUtilsMessengerEXT(vk::Instance instance,
                                        const vk::DebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                        const vk::AllocationCallbacks *pAllocator,
                                        vk::DebugUtilsMessengerEXT *pDebugMessenger);

void DestroyDebugUtilsMessengerEXT(vk::Instance instance, vk::DebugUtilsMessengerEXT debugMessenger, const vk::AllocationCallbacks *pAllocator);

void populateDebugMessengerCreateInfo(vk::DebugUtilsMessengerCreateInfoEXT &createInfo);