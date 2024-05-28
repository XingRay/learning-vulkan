//
// Created by leixing on 2024/5/12.
//

#pragma once

#include <cstdint>
#include <limits>
#include <algorithm>

// 强制 GLM 所有接受角度参数的函数都使用弧度，而不是度数, 以避免任何可能的混淆。
#define GLM_FORCE_RADIANS

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <chrono>

// That way GLFW will include its own definitions and automatically load the Vulkan header with it.
#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>

#include "vulkan/vulkan.hpp"
#include "QueueFamilyIndices.h"
#include "SwapChainSupportDetail.h"

struct Vertex {
    // 位置
    glm::vec2 pos;

    // 颜色
    glm::vec3 color;

    // 纹理坐标
    glm::vec2 texCoord;

    static vk::VertexInputBindingDescription getBindingDescription() {
        vk::VertexInputBindingDescription description;

        // 绑定描述
        description.binding = 0;

        // 步幅，表示每个顶点数据的字节数
        description.stride = sizeof(Vertex);

        // 输入速率，表示每个顶点数据的读取频率
        description.inputRate = vk::VertexInputRate::eVertex;

        return description;
    }

    static std::array<vk::VertexInputAttributeDescription, 3> getAttributeDescriptions() {
        std::array<vk::VertexInputAttributeDescription, 3> attributeDescriptions{};

        // 位置
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = vk::Format::eR32G32Sfloat;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);

        // 颜色
        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = vk::Format::eR32G32B32Sfloat;
        attributeDescriptions[1].offset = offsetof(Vertex, color);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = vk::Format::eR32G32Sfloat;
        attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

        return attributeDescriptions;
    }
};


struct UniformBufferObject {
    // alignas(16) 显示指定字段的对齐方式，
    // 标量必须按N对齐（对于32位浮点数，N = 4字节）。
    // vec2必须按2N对齐（= 8字节）。
    // vec3或vec4必须按4N对齐（= 16字节）。
    // 嵌套结构必须按其成员的基准对齐向上取整至16的倍数对齐。
    // mat4矩阵必须与vec4具有相同的对齐方式。
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
};

class TriangleTest {
public:
    // public fields
private:
    // private fields
    const int32_t mWidth = (int32_t) (1024 * 1.118);
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

    const std::vector<Vertex> mVertices = {
            {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
            {{0.5f,  -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
            {{0.5f,  0.5f},  {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
            {{-0.5f, 0.5f},  {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
    };

    const std::vector<uint16_t> mIndices = {
            0, 1, 2,
            2, 3, 0
    };

    vk::Buffer mVertexBuffer;

    vk::DeviceMemory mVertexBufferMemory;

    vk::Buffer mIndexBuffer;

    vk::DeviceMemory mIndexBufferMemory;

    std::vector<vk::Buffer> mUniformBuffers;
    std::vector<vk::DeviceMemory> mUniformBufferMemories;
    std::vector<void *> mUniformBuffersMapped;

    vk::DescriptorPool mDescriptorPool;

    std::vector<vk::DescriptorSet> mDescriptorSets;

    vk::Image mTextureImage;
    vk::DeviceMemory mTextureImageMemory;

    vk::ImageView mTextureImageView;

    vk::Sampler mTextureSampler;

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

    std::pair<vk::Image, vk::DeviceMemory> createImage(uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling imageTiling,
                                                       vk::ImageUsageFlags imageUsage, vk::MemoryPropertyFlags memoryProperty);

    vk::CommandBuffer beginSingleTimeCommands();

    void endSingleTimeCommands(vk::CommandBuffer &commandBuffer);

    void transitionImageLayout(vk::Image image, vk::Format format, vk::ImageLayout oldImageLayout, vk::ImageLayout newImageLayout);

    void copyBufferToImage(vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height);

    void createTextureImageView();

    vk::ImageView createImageView(const vk::Image &image, const vk::Format &format);

    void createTextureSampler();
};


vk::Result CreateDebugUtilsMessengerEXT(vk::Instance instance,
                                        const vk::DebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                        const vk::AllocationCallbacks *pAllocator,
                                        vk::DebugUtilsMessengerEXT *pDebugMessenger);

void DestroyDebugUtilsMessengerEXT(vk::Instance instance, vk::DebugUtilsMessengerEXT debugMessenger, const vk::AllocationCallbacks *pAllocator);

void populateDebugMessengerCreateInfo(vk::DebugUtilsMessengerCreateInfoEXT &createInfo);