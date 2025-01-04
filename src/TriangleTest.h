//
// Created by leixing on 2024/5/12.
//

#pragma once

#include <cstdint>
#include <limits>
#include <algorithm>

#include <chrono>

// That way GLFW will include its own definitions and automatically load the Vulkan header with it.
#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>

#include "vulkan/vulkan.hpp"
#include "QueueFamilyIndices.h"
#include "SwapChainSupportDetail.h"
#include "ShaderData.h"

#include "VulkanInstance.h"
#include "VulkanSurface.h"
#include "VulkanDevice.h"
#include "VulkanSwapchain.h"
#include "VulkanFrameBuffer.h"
#include "VulkanRenderPass.h"
#include "VulkanDescriptorSet.h"
#include "VulkanPipeline.h"
#include "VulkanCommandPool.h"
#include "VulkanFrameBuffer.h"
#include "VulkanVertexBuffer.h"

namespace triangle {
    class TriangleTest {
    public:
        // public fields
    private:
        // private fields
        const int32_t mWidth = 1024;
        const int32_t mHeight = 1024;
        const std::array<float, 4> mClearColor = {0.05f, 0.05f, 0.05f, 1.0f};


        // 同时处理的帧数
        const int MAX_FRAMES_IN_FLIGHT = 2;

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

        std::unique_ptr<engine::VulkanInstance> mVulkanInstance;
        std::unique_ptr<engine::VulkanSurface> mVulkanSurface;
        std::unique_ptr<engine::VulkanDevice> mVulkanDevice;
        std::unique_ptr<engine::VulkanSwapchain> mVulkanSwapchain;
        std::unique_ptr<engine::VulkanRenderPass> mVulkanRenderPass;
        std::unique_ptr<engine::VulkanDescriptorSet> mVulkanDescriptorSet;
        std::unique_ptr<engine::VulkanPipeline> mVulkanPipeline;
        std::unique_ptr<engine::VulkanCommandPool> mVulkanCommandPool;
        std::unique_ptr<engine::VulkanFrameBuffer> mVulkanFrameBuffer;
        std::unique_ptr<engine::VulkanVertexBuffer> mVulkanVertexBuffer;

        vk::DebugUtilsMessengerEXT mDebugMessenger;

        std::vector<vk::Semaphore> mImageAvailableSemaphores;

        std::vector<vk::Semaphore> mRenderFinishedSemaphores;

        std::vector<vk::Fence> mInFlightFences;

        uint32_t mCurrentFrame = 0;

        bool mFrameBufferResized = false;

        std::vector<app::Vertex> mVertices;

        std::vector<uint32_t> mIndices;

        vk::Buffer mVertexBuffer;

        vk::DeviceMemory mVertexBufferMemory;

        vk::Buffer mIndexBuffer;

        vk::DeviceMemory mIndexBufferMemory;

        vk::Image mDepthImage;
        vk::DeviceMemory mDepthDeviceMemory;
        vk::ImageView mDepthImageView;

        vk::Image mColorImage;
        vk::DeviceMemory mColorDeviceMemory;
        vk::ImageView mColorImageView;

    public:
        TriangleTest();

        ~TriangleTest();

        void main();

    private:

        void initWindow();

        void initVulkan();

        void mainLoop();

        void cleanUp();

        void createInstance();

        std::vector<const char *> getRequiredExtensions();

        void setupDebugMessenger();

        void destroyDebugUtilsMessengerExt(const vk::AllocationCallbacks *pAllocator);

        void createLogicalDevice();

        QueueFamilyIndices findQueueFamilies(vk::PhysicalDevice &device);

        void createSurface();

        void createSwapChain();

        void createGraphicsPipeline();

        void createRenderPass();

        void createFrameBuffers();

        void cleanFrameBuffers();

        void createCommandPool();

        void recordCommandBuffer(const vk::CommandBuffer &commandBuffer, uint32_t imageIndex);

        void drawFrame();

        void createSyncObjects();

        void recreateSwapChain();

        void cleanUpSwapChain();

        void createVertexBuffer();

        void createIndexBuffer();

        uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);

        std::pair<vk::Buffer, vk::DeviceMemory> createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties);

        void copyBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size);

        void createDescriptorSetLayout();

        std::pair<vk::Image, vk::DeviceMemory> createImage(uint32_t width, uint32_t height, uint32_t mipLevels, vk::SampleCountFlagBits numSamples,
                                                           vk::Format format, vk::ImageTiling imageTiling,
                                                           vk::ImageUsageFlags imageUsage, vk::MemoryPropertyFlags memoryProperty);

        vk::CommandBuffer beginSingleTimeCommands();

        void endSingleTimeCommands(vk::CommandBuffer &commandBuffer);

        void transitionImageLayout(vk::Image image, vk::Format format, vk::ImageLayout oldImageLayout, vk::ImageLayout newImageLayout, uint32_t mipLevels);

        vk::ImageView createImageView(const vk::Image &image, vk::Format format, vk::ImageAspectFlags imageAspect, uint32_t mipLevels);

        void createDepthResources();

        vk::Format findSupportedFormat(const std::vector<vk::Format> &candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features);

        vk::Format findDepthFormat();

        bool hasStencilComponent(vk::Format format);

        void cleanDepthResources();

        void loadModel();

        vk::SampleCountFlagBits getMaxUsableSampleCount();

        void createColorResources();

        void cleanColorResources();
    };


    vk::Result CreateDebugUtilsMessengerEXT(vk::Instance instance,
                                            const vk::DebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                            const vk::AllocationCallbacks *pAllocator,
                                            vk::DebugUtilsMessengerEXT *pDebugMessenger);

    void DestroyDebugUtilsMessengerEXT(vk::Instance instance, vk::DebugUtilsMessengerEXT debugMessenger, const vk::AllocationCallbacks *pAllocator);

    void populateDebugMessengerCreateInfo(vk::DebugUtilsMessengerCreateInfoEXT &createInfo);
}
