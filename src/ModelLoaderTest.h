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
#include "TestData.h"

namespace model_loader_test {
    class ModelLoaderTest {
    public:
        // public fields
    private:
        // private fields
        const int32_t mWidth = (int32_t) (1024 * 1.118);
        const int32_t mHeight = 1024;
        const std::array<float, 4> mClearColor = {0.05f, 0.05f, 0.05f, 1.0f};

        const char *MODEL_PATH = "../model/viking_room/viking_room.obj";
        const char *TEXTURE_PATH = "../model/viking_room/viking_room.png";

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

        vk::Instance mInstance;

        vk::DebugUtilsMessengerEXT mDebugMessenger;

        // 物理设备
        vk::PhysicalDevice mPhysicalDevice;

        // 逻辑设备
        vk::Device mDevice;

        // 图形相关的任务队列
        vk::Queue mGraphicsQueue;

        // 显示相关的任务队列
        vk::Queue mPresentQueue;

        vk::SurfaceKHR mSurface;

        vk::SwapchainKHR mSwapChain;

        vk::SurfaceFormatKHR mSwapChainImageFormat;

        vk::Extent2D mSwapChainExtent;

        std::vector<vk::Image> mSwapChainImages;

        std::vector<vk::ImageView> mSwapChainImageViews;

        vk::RenderPass mRenderPass;

        vk::DescriptorSetLayout mDescriptorSetLayout;

        vk::PipelineLayout mPipelineLayout;

        vk::Pipeline mGraphicsPipeline;

        std::vector<vk::Framebuffer> mSwapChainFrameBuffers;

        vk::CommandPool mCommandPool;


        // todo mCommandBuffers mImageAvailableSemaphores mRenderFinishedSemaphores mInFlightFences 组织成对象数组
        std::vector<vk::CommandBuffer> mCommandBuffers;

        std::vector<vk::Semaphore> mImageAvailableSemaphores;

        std::vector<vk::Semaphore> mRenderFinishedSemaphores;

        std::vector<vk::Fence> mInFlightFences;

        uint32_t mCurrentFrame = 0;

        bool mFrameBufferResized = false;

        std::vector<Vertex> mVertices;

        std::vector<uint32_t> mIndices;

        vk::Buffer mVertexBuffer;

        vk::DeviceMemory mVertexBufferMemory;

        vk::Buffer mIndexBuffer;

        vk::DeviceMemory mIndexBufferMemory;

        std::vector<vk::Buffer> mUniformBuffers;
        std::vector<vk::DeviceMemory> mUniformBufferMemories;
        std::vector<void *> mUniformBuffersMapped;

        vk::DescriptorPool mDescriptorPool;

        std::vector<vk::DescriptorSet> mDescriptorSets;

        uint32_t mMipLevels;
        vk::Image mTextureImage;
        vk::DeviceMemory mTextureImageMemory;

        vk::ImageView mTextureImageView;

        vk::Sampler mTextureSampler;

        vk::Image mDepthImage;
        vk::DeviceMemory mDepthDeviceMemory;
        vk::ImageView mDepthImageView;

        vk::SampleCountFlagBits mMsaaSamples = vk::SampleCountFlagBits::e1;

        vk::Image mColorImage;
        vk::DeviceMemory mColorDeviceMemory;
        vk::ImageView mColorImageView;

    public:
        ModelLoaderTest();

        ~ModelLoaderTest();

        void main();

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

        void createLogicalDevice();

        QueueFamilyIndices findQueueFamilies(vk::PhysicalDevice &device);

        void createSurface();

        bool isDeviceSupportedRequiredExtensions(vk::PhysicalDevice device);

        SwapChainSupportDetail querySwapChainSupported(vk::PhysicalDevice &device);

        vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &availableFormats);

        vk::PresentModeKHR choosePresentMode(const std::vector<vk::PresentModeKHR> &availablePresentModes);

        vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR &capability);

        void createSwapChain();

        void createImageViews();

        void cleanImageViews();

        void createGraphicsPipeline();

        vk::ShaderModule createShaderModule(const std::vector<char> &code);

        void createRenderPass();

        void createFrameBuffers();

        void cleanFrameBuffers();

        void createCommandPool();

        void createCommandBuffers();

        void recordCommandBuffer(const vk::CommandBuffer &commandBuffer, uint32_t imageIndex);

        void drawFrame();

        void createSyncObjects();

        void cleanSyncObjects();

        void recreateSwapChain();

        void cleanUpSwapChain();

        void createVertexBuffer();

        void createIndexBuffer();

        uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);

        std::pair<vk::Buffer, vk::DeviceMemory> createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties);

        void copyBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size);


        void createDescriptorSetLayout();

        void createUniformBuffers();

        void cleanUniformBuffers();

        void updateUniformBuffer(uint32_t frameIndex);

        void createDescriptorPool();

        void createDescriptorSets();

        void createTextureImage();

        std::pair<vk::Image, vk::DeviceMemory> createImage(uint32_t width, uint32_t height, uint32_t mipLevels, vk::SampleCountFlagBits numSamples,
                                                           vk::Format format, vk::ImageTiling imageTiling,
                                                           vk::ImageUsageFlags imageUsage, vk::MemoryPropertyFlags memoryProperty);

        vk::CommandBuffer beginSingleTimeCommands();

        void endSingleTimeCommands(vk::CommandBuffer &commandBuffer);

        void transitionImageLayout(vk::Image image, vk::Format format, vk::ImageLayout oldImageLayout, vk::ImageLayout newImageLayout, uint32_t mipLevels);

        void copyBufferToImage(vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height);

        void createTextureImageView();

        vk::ImageView createImageView(const vk::Image &image, vk::Format format, vk::ImageAspectFlags imageAspect, uint32_t mipLevels);

        void createTextureSampler();

        void createDepthResources();

        vk::Format findSupportedFormat(const std::vector<vk::Format> &candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features);

        vk::Format findDepthFormat();

        bool hasStencilComponent(vk::Format format);

        void cleanDepthResources();

        void loadModel();

        void generateMipmaps(vk::Image image, vk::Format imageFormat, int textureWidth, int textureHeight, uint32_t mipLevels);

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
