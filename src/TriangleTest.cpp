//
// Created by leixing on 2024/5/12.
//

#include "TriangleTest.h"
#include "vulkan/vulkan.hpp"

#include <iostream>
#include <map>
#include <set>
#include <string>
#include <unordered_map>

#include "QueueFamilyIndices.h"
#include "FileUtil.h"

// https://github.com/nothings/stb/issues/917
// #define STB_IMAGE_IMPLEMENTATION 不能放在头文件中， 必须放在源文件中
#define STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>

//#define STB_IMAGE_WRITE_IMPLEMENTATION
//#include "stb_image_write.h"

//#define STB_IMAGE_RESIZE_IMPLEMENTATION
//#include "stb_image_resize.h"

#define TINYOBJLOADER_IMPLEMENTATION

#include <tiny_obj_loader.h>


TriangleTest::TriangleTest() {

}

TriangleTest::~TriangleTest() {

}

void TriangleTest::run() {

#if defined(VK_VERSION_1_0)
    std::cout << "VK_VERSION_1_0 defined" << std::endl;
#elif
    std::cout<<"VK_VERSION_1_0 not defined"<<std::endl;
#endif

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
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    // The first three parameters specify the width, height and title of the mWindow.
    // The fourth parameter allows you to optionally specify a monitor to open the mWindow on
    // and the last parameter is only relevant to OpenGL.
    mWindow = glfwCreateWindow(mWidth, mHeight, "triangle test", nullptr, nullptr);
    // 将 this 指针保存到window对象中， 这样可以在callback中取出， 这里使用 lambda， 可以不需要
    glfwSetWindowUserPointer(mWindow, this);
    glfwSetFramebufferSizeCallback(mWindow, [](GLFWwindow *window, int width, int height) {
        auto app = reinterpret_cast<TriangleTest *>(glfwGetWindowUserPointer(window));
        std::cout << "app->mFrameBufferResized, width:" << width << " height:" << height << std::endl;
        app->mFrameBufferResized = true;
    });
}

void TriangleTest::initVulkan() {
    createInstance();
    setupDebugMessenger();
    createSurface();
    pickPhysicalDevice();
    createLogicalDevice();
    createSwapChain();
    createImageViews();
    createRenderPass();
    createDescriptorSetLayout();
    createGraphicsPipeline();
    createCommandPool();
    createColorResources();
    createDepthResources();
    createFrameBuffers();
    createTextureImage();
    createTextureImageView();
    createTextureSampler();
    loadModel();
    createVertexBuffer();
    createIndexBuffer();
    createUniformBuffers();
    createDescriptorPool();
    createDescriptorSets();
    createCommandBuffers();
    createSyncObjects();
}

void TriangleTest::mainLoop() {
    while (!glfwWindowShouldClose(mWindow)) {
        glfwPollEvents();
        drawFrame();
    }

    // 等待所有任务执行完成
    mDevice.waitIdle();
}

void TriangleTest::cleanUp() {
    cleanUpSwapChain();

    mDevice.destroy(mTextureSampler);
    mDevice.destroy(mTextureImageView);

    mDevice.destroy(mTextureImage);
    mDevice.freeMemory(mTextureImageMemory);

    cleanUniformBuffers();
    mDevice.destroy(mDescriptorPool);
    mDevice.destroy(mDescriptorSetLayout);

    mDevice.destroy(mVertexBuffer);
    mDevice.free(mVertexBufferMemory);

    mDevice.destroy(mIndexBuffer);
    mDevice.free(mIndexBufferMemory);

    mDevice.destroy(mGraphicsPipeline);
    mDevice.destroy(mPipelineLayout);

    mDevice.destroy(mRenderPass);

    cleanSyncObjects();
    mDevice.destroy(mCommandPool);

    mDevice.destroy();

    if (mEnableValidationLayer) {
        destroyDebugUtilsMessengerExt(nullptr);
    }
    mInstance.destroy(mSurface);
    mInstance.destroy();

    glfwDestroyWindow(mWindow);
    glfwTerminate();
}

void TriangleTest::createInstance() {
    std::cout << "createInstance\n";

    vk::ApplicationInfo appInfo;
//    appInfo.setPApplicationName("triangle test");
//    appInfo.setApplicationVersion(VK_MAKE_VERSION(1, 0, 0));
//    appInfo.setPEngineName("no engine");
//    appInfo.setEngineVersion(VK_MAKE_VERSION(1, 0, 0));
    appInfo.setApiVersion(VK_API_VERSION_1_3);

    vk::InstanceCreateInfo createInfo;
    createInfo.setPApplicationInfo(&appInfo);

    std::vector<const char *> extensions = getRequiredExtensions();
//    createInfo.setEnabledExtensionCount(extensions.size());
    createInfo.setPEnabledExtensionNames(extensions);
    std::cout << "extensions \n";
    for (auto &extension: extensions) {
        std::cout << '\t' << extension << std::endl;
    }

    vk::DebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if (mEnableValidationLayer) {
        if (!checkValidationLayerSupported()) {
            throw std::runtime_error("validation layers required, but not available !");
        }
        createInfo.setPEnabledLayerNames(mValidationLayers);

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

//    std::cout << "available extensions \n";
//    for (const auto &extension: allExtensions) {
//        std::cout << '\t' << extension.extensionName << '\n';
//    }
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
    // 查询 glfw 需要的 vulkan 扩展
    const char **glfwRequiredExtensions = glfwGetRequiredInstanceExtensions(&glfwRequiredExtensionCount);
    std::cout << "glfwRequiredExtensions:" << std::endl;
    for (int i = 0; i < glfwRequiredExtensionCount; i++) {
        std::cout << '\t' << glfwRequiredExtensions[i] << std::endl;
    }
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
    std::cout << "setupDebugMessenger\n";
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
    std::cout << "pickPhysicalDevice\n";
    auto devices = mInstance.enumeratePhysicalDevices();
    for (const auto &device: devices) {
        vk::PhysicalDeviceProperties properties = device.getProperties();
        std::cout << "\tdeviceName: " << properties.deviceName << "\tdeviceType: " << to_string(properties.deviceType) << std::endl;
    }

    // 简单查找需要的设备
    for (const auto &device: devices) {
        if (isDeviceSuitable(device)) {
            mPhysicalDevice = device;
            mMsaaSamples = getMaxUsableSampleCount();
            break;
        }
    }

    if (mPhysicalDevice == VK_NULL_HANDLE) {
        std::cout << "mPhysicalDevice not found" << std::endl;
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

//    auto extensionProperties = mPhysicalDevice.enumerateDeviceExtensionProperties();
//    for (const auto &extensionProperty: extensionProperties) {
//        std::cout << "\textensionName: " << extensionProperty.extensionName << "\tspecVersion: " << extensionProperty.specVersion << std::endl;
//    }
}

bool TriangleTest::isDeviceSuitable(vk::PhysicalDevice device) {
    vk::PhysicalDeviceProperties deviceProperties = device.getProperties();
    vk::PhysicalDeviceFeatures deviceFeatures = device.getFeatures();

    QueueFamilyIndices indices = findQueueFamilies(device);
    if (!indices.isComplete()) {
        std::cout << "device QueueFamilyIndices is not complete !" << std::endl;
        return false;
    }

    if (!isDeviceSupportedRequiredExtensions(device)) {
        std::cout << "isDeviceSupportedRequiredExtensions: false" << std::endl;
        return false;
    }

    // 验证扩展可用后才尝试查询交换链支持
    SwapChainSupportDetail swapChainSupportedDetail = querySwapChainSupported(device);
    if (swapChainSupportedDetail.formats.empty() || swapChainSupportedDetail.presentModes.empty()) {
        std::cout << "swapChainSupportedDetail: formats or presentModes is empty" << std::endl;
        return false;
    }

//     核显
//    vk::PhysicalDeviceType::eIntegratedGpu;

//    独显
//    vk::PhysicalDeviceType::eDiscreteGpu;
    return deviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu && deviceFeatures.geometryShader && deviceFeatures.samplerAnisotropy;
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

void TriangleTest::createLogicalDevice() {
    std::cout << "createLogicalDevice\n";
    QueueFamilyIndices indices = findQueueFamilies(mPhysicalDevice);
    if (!indices.isComplete()) {
        std::cout << "findQueueFamilies failed, indices is not complete" << std::endl;
        throw std::runtime_error("findQueueFamilies failed");
    }
    uint32_t graphicFamilyIndex = indices.graphicQueueFamily.value();
    uint32_t presentFamilyIndex = indices.graphicQueueFamily.value();

    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
    float queuePriority = 1.0f;
    if (graphicFamilyIndex == presentFamilyIndex) {
        vk::DeviceQueueCreateInfo queueCreateInfo;
        queueCreateInfo.queueFamilyIndex = graphicFamilyIndex;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;

        queueCreateInfos.push_back(queueCreateInfo);
    } else {
        std::array<uint32_t, 2> queueFamilyIndices = {graphicFamilyIndex, presentFamilyIndex};
        for (uint32_t queueFamilyIndex: queueFamilyIndices) {
            vk::DeviceQueueCreateInfo queueCreateInfo;
            queueCreateInfo.queueFamilyIndex = queueFamilyIndex;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }
    }


    vk::PhysicalDeviceFeatures deviceFeatures{};
    deviceFeatures.setSamplerAnisotropy(vk::True)
            .setSampleRateShading(vk::True);

    vk::DeviceCreateInfo deviceCreateInfo;
    deviceCreateInfo.setPQueueCreateInfos(queueCreateInfos.data())
            .setQueueCreateInfoCount(queueCreateInfos.size())
            .setPEnabledFeatures(&deviceFeatures)
            .setEnabledExtensionCount(mRequiredExtensions.size())
            .setPpEnabledExtensionNames(mRequiredExtensions.data());

    if (mEnableValidationLayer) {
        deviceCreateInfo.enabledLayerCount = mValidationLayers.size();
        deviceCreateInfo.setPpEnabledLayerNames(mValidationLayers.data());
    } else {
        deviceCreateInfo.enabledLayerCount = 0;
    }

    // 创建逻辑设备的同时会根据 deviceCreateInfo.pQueueCreateInfo 创建任务队列
    mDevice = mPhysicalDevice.createDevice(deviceCreateInfo);
    // 从逻辑设备中取出任务队列, 第二个参数为下标, 总共创建了一个队列,所以这里下标为 0
    mGraphicsQueue = mDevice.getQueue(graphicFamilyIndex, 0);
    mPresentQueue = mDevice.getQueue(presentFamilyIndex, 0);
}

QueueFamilyIndices TriangleTest::findQueueFamilies(vk::PhysicalDevice &device) {
    QueueFamilyIndices indices;
    auto queueFamilyProperties = device.getQueueFamilyProperties();


    for (int i = 0; i < queueFamilyProperties.size(); i++) {
        const auto &queueFamilyProperty = queueFamilyProperties[i];
        const vk::QueueFlags &queueFlags = queueFamilyProperty.queueFlags;

        if (queueFlags & vk::QueueFlagBits::eGraphics) {
            std::cout << "graphicQueueFamily found: " << i << std::endl;
            indices.graphicQueueFamily = i;
        }

        if (device.getSurfaceSupportKHR(i, mSurface)) {
            std::cout << "presentQueueFamily found: " << i << std::endl;
            indices.presentQueueFamily = i;
        }

        if (indices.isComplete()) {
            break;
        }
    }

    return indices;
}

void TriangleTest::createSurface() {
    std::cout << "createSurface\n";
    VkResult result = glfwCreateWindowSurface((VkInstance) mInstance, mWindow, nullptr, (VkSurfaceKHR *) &mSurface);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }
    std::cout << "glfwCreateWindowSurface success !" << std::endl;
}

bool TriangleTest::isDeviceSupportedRequiredExtensions(vk::PhysicalDevice device) {
    auto properties = device.enumerateDeviceExtensionProperties();
    std::set<std::string> requiredExtensions(mRequiredExtensions.begin(), mRequiredExtensions.end());
    for (const auto &property: properties) {
        requiredExtensions.erase(property.extensionName);
    }
    return requiredExtensions.empty();
}

SwapChainSupportDetail TriangleTest::querySwapChainSupported(vk::PhysicalDevice &device) {
    SwapChainSupportDetail detail;

    detail.capabilities = device.getSurfaceCapabilitiesKHR(mSurface);
    detail.formats = device.getSurfaceFormatsKHR(mSurface);
    detail.presentModes = device.getSurfacePresentModesKHR(mSurface);

    return detail;
}

vk::SurfaceFormatKHR TriangleTest::chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &availableFormats) {
    for (const auto &availableFormat: availableFormats) {
        if (availableFormat.format == vk::Format::eR8G8B8A8Srgb && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

vk::PresentModeKHR TriangleTest::choosePresentMode(const std::vector<vk::PresentModeKHR> &availablePresentModes) {
    for (const auto &availablePresentMode: availablePresentModes) {
        if (availablePresentMode == vk::PresentModeKHR::eMailbox) {
            return availablePresentMode;
        }
    }
    return vk::PresentModeKHR::eFifo;
}

vk::Extent2D TriangleTest::chooseSwapExtent(const vk::SurfaceCapabilitiesKHR &capability) {
    if (capability.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capability.currentExtent;
    }

    int width;
    int height;

    glfwGetFramebufferSize(mWindow, &width, &height);
    return vk::Extent2D{
            std::clamp((uint32_t) width, capability.minImageExtent.width, capability.maxImageExtent.width),
            std::clamp((uint32_t) height, capability.minImageExtent.height, capability.maxImageExtent.height),
    };
}

void TriangleTest::createSwapChain() {
    SwapChainSupportDetail supportDetail = querySwapChainSupported(mPhysicalDevice);
    vk::SurfaceCapabilitiesKHR capabilities = supportDetail.capabilities;

    mSwapChainImageFormat = chooseSwapSurfaceFormat(supportDetail.formats);
    vk::PresentModeKHR presentMode = choosePresentMode(supportDetail.presentModes);
    mSwapChainExtent = chooseSwapExtent(capabilities);

    uint32_t imageCount = capabilities.minImageCount + 1;
    // capabilities.maxImageCount == 0 表示不做限制
    if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount) {
        imageCount = capabilities.maxImageCount;
    }
    std::cout << "imageCount: " << imageCount << std::endl;

    vk::SwapchainCreateInfoKHR createInfo{};
    createInfo.surface = mSurface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = mSwapChainImageFormat.format;
    createInfo.imageColorSpace = mSwapChainImageFormat.colorSpace;
    createInfo.imageExtent = mSwapChainExtent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;

    QueueFamilyIndices queueFamilyIndices = findQueueFamilies(mPhysicalDevice);
    if (queueFamilyIndices.graphicQueueFamily == queueFamilyIndices.presentQueueFamily) {
        // 共用一个队列, 图片可以被队列独占, 这样效率最高
        createInfo.imageSharingMode = vk::SharingMode::eExclusive;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.setQueueFamilyIndices(nullptr);
    } else {
        // 2个队列
        createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
        createInfo.queueFamilyIndexCount = 2;
        uint32_t indices[]{queueFamilyIndices.graphicQueueFamily.value(), queueFamilyIndices.presentQueueFamily.value()};
        createInfo.setPQueueFamilyIndices(indices);
    }

    createInfo.preTransform = capabilities.currentTransform;
    createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
    createInfo.presentMode = presentMode;
    createInfo.clipped = vk::True;

    createInfo.oldSwapchain = VK_NULL_HANDLE;

    mSwapChain = mDevice.createSwapchainKHR(createInfo);
    // swapChainImages 由 swapChain 管理, swapChain销毁时, swapChainImages 会自动销毁
    mSwapChainImages = mDevice.getSwapchainImagesKHR(mSwapChain);
}

void TriangleTest::createImageViews() {
    mSwapChainImageViews.resize(mSwapChainImages.size());
    for (int i = 0; i < mSwapChainImages.size(); i++) {
        mSwapChainImageViews[i] = createImageView(mSwapChainImages[i], mSwapChainImageFormat.format, vk::ImageAspectFlagBits::eColor, 1);
    }
}

void TriangleTest::cleanImageViews() {
    for (const auto &imageView: mSwapChainImageViews) {
        mDevice.destroy(imageView);
    }
}

void TriangleTest::createRenderPass() {
    vk::AttachmentDescription colorAttachmentDescription{};
    colorAttachmentDescription.setFormat(mSwapChainImageFormat.format)
            .setSamples(mMsaaSamples)
                    //载入图像前将帧缓冲清0
            .setLoadOp(vk::AttachmentLoadOp::eClear)
                    // 渲染图像之后将图像数据保存
            .setStoreOp(vk::AttachmentStoreOp::eStore)
                    // 模版缓冲, 这里不关注
            .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
            .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
                    // 常见的布局
                    //
                    //VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL：用作彩色附件的图像
                    //VK_IMAGE_LAYOUT_PRESENT_SRC_KHR：要在交换链中呈现的图像
                    //VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL：用作内存复制操作目标的图像
                    //
                    // initialLayout 渲染通道开始之前图像将具有的布局
                    // finalLayout 渲染通道完成时自动转换到的布局
                    //
                    // 使用 VK_IMAGE_LAYOUT_UNDEFINED 意味着我们不关心图像以前的布局
                    // 这个特殊值的警告是图像的内容不能保证被保留，但这并不重要，因为我们无论如何要清除
            .setInitialLayout(vk::ImageLayout::eUndefined)
                    // 我们希望图像在渲染后准备好使用交换链进行呈现，所以我们设置finalLayout 为 VK_IMAGE_LAYOUT_PRESENT_SRC_KHRas
            .setFinalLayout(vk::ImageLayout::eColorAttachmentOptimal);

    vk::AttachmentReference colorAttachmentReference{};
    // 多个 colorAttachmentDescription 组成数组, 上面只有一个 colorAttachmentDescription, 那么下标为 0
    colorAttachmentReference.setAttachment(0)
            .setLayout(vk::ImageLayout::eColorAttachmentOptimal);


    vk::AttachmentDescription depthAttachmentDescription{};
    depthAttachmentDescription.setFormat(findDepthFormat())
            .setSamples(mMsaaSamples)
            .setLoadOp(vk::AttachmentLoadOp::eClear)
            .setStoreOp(vk::AttachmentStoreOp::eDontCare)
            .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
            .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
            .setInitialLayout(vk::ImageLayout::eUndefined)
            .setFinalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

    vk::AttachmentReference depthAttachmentReference{};
    depthAttachmentReference.setAttachment(1)
            .setLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

    vk::AttachmentDescription colorAttachmentResolveDescription{};
    colorAttachmentResolveDescription.setFormat(mSwapChainImageFormat.format)
            .setSamples(vk::SampleCountFlagBits::e1)
            .setLoadOp(vk::AttachmentLoadOp::eDontCare)
            .setStoreOp(vk::AttachmentStoreOp::eStore)
            .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
            .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
            .setInitialLayout(vk::ImageLayout::eUndefined)
            .setFinalLayout(vk::ImageLayout::ePresentSrcKHR);

    vk::AttachmentReference colorAttachmentResolveReference{};
    colorAttachmentResolveReference.setAttachment(2)
            .setLayout(vk::ImageLayout::eColorAttachmentOptimal);

    vk::SubpassDescription subpassDescription{};
    subpassDescription.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
            .setColorAttachmentCount(1)
            .setPColorAttachments(&colorAttachmentReference)
            .setPDepthStencilAttachment(&depthAttachmentReference)
            .setPResolveAttachments(&colorAttachmentResolveReference);


    vk::SubpassDependency subpassDependency{};
    subpassDependency.setSrcSubpass(vk::SubpassExternal);
    subpassDependency.setDstSubpass(0);
    subpassDependency.setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests);
    subpassDependency.setSrcAccessMask(vk::AccessFlagBits::eNone);
    subpassDependency.setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests);
    subpassDependency.setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite);

    std::array<vk::AttachmentDescription, 3> attachments = {colorAttachmentDescription, depthAttachmentDescription, colorAttachmentResolveDescription};
    vk::RenderPassCreateInfo renderPassCreateInfo{};
    renderPassCreateInfo
//            .setAttachmentCount(attachments.size())
            .setAttachments(attachments)
            .setSubpassCount(1)
            .setPSubpasses(&subpassDescription)
            .setDependencyCount(1)
            .setPDependencies(&subpassDependency);

    mRenderPass = mDevice.createRenderPass(renderPassCreateInfo);
}

void TriangleTest::createGraphicsPipeline() {
    auto vertexShaderCode = FileUtil::readFile("../output/shader/vertex.spv");
    auto fragmentShaderCode = FileUtil::readFile("../output/shader/fragment.spv");

    vk::ShaderModule vertexModule = createShaderModule(vertexShaderCode);
    vk::ShaderModule fragmentModule = createShaderModule(fragmentShaderCode);

    // input assembler
    vk::PipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo;
    inputAssemblyStateCreateInfo.setTopology(vk::PrimitiveTopology::eTriangleList)
            .setPrimitiveRestartEnable(vk::False);

    std::vector<vk::DynamicState> dynamicStages = {
            vk::DynamicState::eViewport,
            vk::DynamicState::eScissor
    };

    vk::PipelineDynamicStateCreateInfo dynamicStateCreateInfo;
    dynamicStateCreateInfo.setDynamicStateCount(dynamicStages.size())
            .setPDynamicStates(dynamicStages.data());

    vk::Viewport viewport;
    viewport.setX(0.0f)
            .setY(0.0f)
            .setWidth((float) mSwapChainExtent.width)
            .setHeight((float) mSwapChainExtent.height)
            .setMinDepth(0.0f)
            .setMaxDepth(1.0f);

    vk::Rect2D scissor{};
    scissor.setOffset(vk::Offset2D{0, 0})
            .setExtent(mSwapChainExtent);

    vk::PipelineViewportStateCreateInfo viewportStateCreateInfo;
    viewportStateCreateInfo.setViewportCount(1)
            .setPViewports(&viewport)
            .setScissorCount(1)
            .setPScissors(&scissor);

    // vertex shader
    vk::VertexInputBindingDescription bindingDescription = Vertex::getBindingDescription();
    std::array<vk::VertexInputAttributeDescription, 3> attributeDescriptions = Vertex::getAttributeDescriptions();
    vk::PipelineVertexInputStateCreateInfo vertexInputStateCreateInfo;
    vertexInputStateCreateInfo.setPVertexBindingDescriptions(&bindingDescription)
            .setVertexBindingDescriptionCount(1)
            .setPVertexAttributeDescriptions(attributeDescriptions.data())
            .setVertexAttributeDescriptionCount(attributeDescriptions.size());
//            .setVertexAttributeDescriptions(attributeDescriptions);

    vk::PipelineShaderStageCreateInfo vertexShaderStageCreateInfo;
    vertexShaderStageCreateInfo.setStage(vk::ShaderStageFlagBits::eVertex)
            .setModule(vertexModule)
            .setPName("main")
            .setPSpecializationInfo(nullptr);

    // tessellation

    // geometry shader

    // rasterization
    vk::PipelineRasterizationStateCreateInfo rasterizationStateCreateInfo;

    // 如果depthClampEnable设置为VK_TRUE，则超出近平面和远平面的片段将被夹紧，而不是丢弃它们。这在某些特殊情况下很有用，例如阴影贴图。使用此功能需要启用 GPU 功能。
    rasterizationStateCreateInfo.setDepthClampEnable(vk::False)
                    // 如果rasterizerDiscardEnable设置为VK_TRUE，则几何图形永远不会通过光栅化阶段。这基本上禁用了帧缓冲区的任何输出。
            .setRasterizerDiscardEnable(vk::False)
                    //确定polygonMode如何为几何体生成片段。可以使用以下模式：
                    //VK_POLYGON_MODE_FILL：用碎片填充多边形区域
                    //VK_POLYGON_MODE_LINE：多边形边缘绘制为线
                    //VK_POLYGON_MODE_POINT：多边形顶点绘制为点
            .setPolygonMode(vk::PolygonMode::eFill)
                    // 使用填充以外的任何模式都需要设置 lineWidth :
            .setLineWidth(1.0f)
                    // 设置面剔除策略, 这里设置为反面被剔除
            .setCullMode(vk::CullModeFlagBits::eBack)
                    // 设置正面的方向, 这设置为顺时针为正面
            .setFrontFace(vk::FrontFace::eCounterClockwise)
            .setDepthBiasEnable(vk::False)
            .setDepthBiasConstantFactor(0.0f)
            .setDepthBiasClamp(0.0f)
            .setDepthBiasSlopeFactor(0.0f);

    // depth & stencil testing
    vk::PipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo;
    depthStencilStateCreateInfo.setDepthTestEnable(vk::True)
            .setDepthWriteEnable(vk::True)
            .setDepthCompareOp(vk::CompareOp::eLess)
            .setDepthBoundsTestEnable(vk::False)
            .setMinDepthBounds(0.0f)
            .setMaxDepthBounds(1.0f)
            .setStencilTestEnable(vk::False)
            .setFront({})
            .setBack({});


    // Multisampling
    vk::PipelineMultisampleStateCreateInfo multisampleStateCreateInfo;
    multisampleStateCreateInfo.setSampleShadingEnable(vk::True)
            .setRasterizationSamples(mMsaaSamples)
            .setMinSampleShading(0.2f)
            .setPSampleMask(nullptr)
            .setAlphaToCoverageEnable(vk::False)
            .setAlphaToOneEnable(vk::False);

    // fragment shader
    vk::PipelineShaderStageCreateInfo fragmentShaderStageCreateInfo;
    fragmentShaderStageCreateInfo.setStage(vk::ShaderStageFlagBits::eFragment)
            .setModule(fragmentModule)
            .setPName("main")
            .setPSpecializationInfo(nullptr);

    // color blending
    vk::PipelineColorBlendAttachmentState colorBlendAttachmentState{};
    colorBlendAttachmentState.setColorWriteMask(vk::ColorComponentFlagBits::eR
                                                | vk::ColorComponentFlagBits::eG
                                                | vk::ColorComponentFlagBits::eB
                                                | vk::ColorComponentFlagBits::eA);
//    colorBlendAttachmentState.setBlendEnable(vk::False);
//
//    // rgb = src.rgb*1 + dst.rgb*0
//    colorBlendAttachmentState.setSrcColorBlendFactor(vk::BlendFactor::eOne);
//    colorBlendAttachmentState.setDstColorBlendFactor(vk::BlendFactor::eZero);
//    colorBlendAttachmentState.setColorBlendOp(vk::BlendOp::eAdd);
//
//    // a = src.a*1+dst.a*1
//    colorBlendAttachmentState.setSrcAlphaBlendFactor(vk::BlendFactor::eOne);
//    colorBlendAttachmentState.setDstAlphaBlendFactor(vk::BlendFactor::eZero);
//    colorBlendAttachmentState.setAlphaBlendOp(vk::BlendOp::eAdd);

    // 伪代码:
    // if (blendEnable) {
    //    finalColor.rgb = (srcColorBlendFactor * newColor.rgb) <colorBlendOp> (dstColorBlendFactor * oldColor.rgb);
    //    finalColor.a = (srcAlphaBlendFactor * newColor.a) <alphaBlendOp> (dstAlphaBlendFactor * oldColor.a);
    //} else {
    //    finalColor = newColor;
    //}
    //
    //finalColor = finalColor & colorWriteMask;

    // 常用的混合模式是透明混合
    // finalColor.rgb = newAlpha * newColor + (1 - newAlpha) * oldColor;
    // finalColor.a = newAlpha.a;
    colorBlendAttachmentState.setBlendEnable(vk::True)
            .setSrcColorBlendFactor(vk::BlendFactor::eSrcAlpha)
            .setDstColorBlendFactor(vk::BlendFactor::eOneMinusSrcAlpha)
            .setColorBlendOp(vk::BlendOp::eAdd)
            .setSrcAlphaBlendFactor(vk::BlendFactor::eOne)
            .setDstAlphaBlendFactor(vk::BlendFactor::eZero)
            .setAlphaBlendOp(vk::BlendOp::eAdd);

    vk::PipelineColorBlendStateCreateInfo colorBlendStateCreateInfo;
    colorBlendStateCreateInfo.setLogicOpEnable(vk::False)
            .setLogicOp(vk::LogicOp::eCopy)
            .setAttachmentCount(1)
            .setPAttachments(&colorBlendAttachmentState)
            .setBlendConstants(std::array<float, 4>{0.0f, 0.0f, 0.0f, 0.0f});


    vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
    pipelineLayoutCreateInfo.setSetLayoutCount(1)
            .setPSetLayouts(&mDescriptorSetLayout)
            .setPushConstantRangeCount(0)
            .setPPushConstantRanges(nullptr);

    mPipelineLayout = mDevice.createPipelineLayout(pipelineLayoutCreateInfo);

    vk::PipelineShaderStageCreateInfo shaderStageCreateInfos[] = {vertexShaderStageCreateInfo, fragmentShaderStageCreateInfo};
    vk::GraphicsPipelineCreateInfo graphicsPipelineCreateInfo;
    graphicsPipelineCreateInfo.setStageCount(2)
            .setPStages(shaderStageCreateInfos)
            .setPVertexInputState(&vertexInputStateCreateInfo)
            .setPInputAssemblyState(&inputAssemblyStateCreateInfo)
            .setPViewportState(&viewportStateCreateInfo)
            .setPRasterizationState(&rasterizationStateCreateInfo)
            .setPMultisampleState(&multisampleStateCreateInfo)
            .setPDepthStencilState(&depthStencilStateCreateInfo)
            .setPColorBlendState(&colorBlendStateCreateInfo)
            .setPDynamicState(&dynamicStateCreateInfo)
            .setLayout(mPipelineLayout)
            .setRenderPass(mRenderPass)
            .setSubpass(0)
            .setBasePipelineHandle(VK_NULL_HANDLE)
            .setBasePipelineIndex(-1);

    auto [result, pipeline] = mDevice.createGraphicsPipeline(VK_NULL_HANDLE, graphicsPipelineCreateInfo);
    if (result != vk::Result::eSuccess) {
        throw std::runtime_error("createGraphicsPipelines failed");
    }
    mGraphicsPipeline = pipeline;

    mDevice.destroy(vertexModule);
    mDevice.destroy(fragmentModule);
}

vk::ShaderModule TriangleTest::createShaderModule(const std::vector<char> &code) {
    vk::ShaderModuleCreateInfo createInfo;
    createInfo.setCodeSize(code.size())
            .setPCode(reinterpret_cast<const uint32_t *>(code.data()));

    return mDevice.createShaderModule(createInfo);
}

void TriangleTest::createFrameBuffers() {
    mSwapChainFrameBuffers.resize(mSwapChainImageViews.size());

    for (int i = 0; i < mSwapChainImageViews.size(); i++) {
        std::array<vk::ImageView, 3> attachments = {
                mColorImageView,
                mDepthImageView,
                mSwapChainImageViews[i],
        };

        vk::FramebufferCreateInfo framebufferCreateInfo{};
        framebufferCreateInfo.setRenderPass(mRenderPass)
//                .setAttachmentCount(attachments.size())
                .setAttachments(attachments)
                .setWidth(mSwapChainExtent.width)
                .setHeight(mSwapChainExtent.height)
                .setLayers(1);

        mSwapChainFrameBuffers[i] = mDevice.createFramebuffer(framebufferCreateInfo);
    }
}

void TriangleTest::createCommandPool() {
    QueueFamilyIndices queueFamilyIndices = findQueueFamilies(mPhysicalDevice);

    vk::CommandPoolCreateInfo commandPoolCreateInfo{};
    commandPoolCreateInfo.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
            .setQueueFamilyIndex(queueFamilyIndices.graphicQueueFamily.value());

    mCommandPool = mDevice.createCommandPool(commandPoolCreateInfo);
}

void TriangleTest::createCommandBuffers() {
    vk::CommandBufferAllocateInfo commandBufferAllocateInfo;
    commandBufferAllocateInfo.setCommandPool(mCommandPool)
            .setLevel(vk::CommandBufferLevel::ePrimary)
            .setCommandBufferCount(MAX_FRAMES_IN_FLIGHT);

    // 返回 vector<CommandBuffer>, 取 [0]
    mCommandBuffers = mDevice.allocateCommandBuffers(commandBufferAllocateInfo);
    // mCommandBuffer 当 CommandPool 被销毁时会自动销毁, 不需要手动销毁
}

void TriangleTest::recordCommandBuffer(const vk::CommandBuffer &commandBuffer, uint32_t imageIndex) {
    vk::CommandBufferBeginInfo commandBufferBeginInfo;
//    commandBufferBeginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
//    commandBufferBeginInfo.setPInheritanceInfo(nullptr);
    commandBuffer.begin(commandBufferBeginInfo);

    vk::Rect2D renderArea;
    renderArea.setOffset(vk::Offset2D{0, 0})
            .setExtent(mSwapChainExtent);

    vk::RenderPassBeginInfo renderPassBeginInfo{};
    renderPassBeginInfo.setRenderPass(mRenderPass)
            .setFramebuffer(mSwapChainFrameBuffers[imageIndex])
            .setRenderArea(renderArea);

    vk::ClearValue colorClearValue = vk::ClearValue{mClearColor};
    std::array<float, 4> depthStencil = {1.0f, 0, 0, 0};
    vk::ClearValue depthStencilClearValue = vk::ClearValue{vk::ClearColorValue(depthStencil)};
    std::array<vk::ClearValue, 2> clearValues = {colorClearValue, depthStencilClearValue};
    renderPassBeginInfo.setClearValueCount(clearValues.size())
            .setClearValues(clearValues);

    commandBuffer.beginRenderPass(&renderPassBeginInfo, vk::SubpassContents::eInline);
    {
        vk::Viewport viewport;
        viewport.setX(0.0f)
                .setY(0.0f)
                .setWidth((float) mSwapChainExtent.width)
                .setHeight((float) mSwapChainExtent.height)
                .setMinDepth(0.0f)
                .setMaxDepth(1.0f);

        vk::Rect2D scissor{};
        scissor.setOffset(vk::Offset2D{0, 0})
                .setExtent(mSwapChainExtent);

        vk::Buffer vertexBuffers[] = {mVertexBuffer};
        vk::DeviceSize offsets[] = {0};

        commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, mGraphicsPipeline);
        commandBuffer.setViewport(0, 1, &viewport);
        commandBuffer.setScissor(0, 1, &scissor);
        commandBuffer.bindVertexBuffers(0, vertexBuffers, offsets);
        commandBuffer.bindIndexBuffer(mIndexBuffer, 0, vk::IndexType::eUint32);
        // vertexCount：即使我们没有顶点缓冲区，从技术上讲我们仍然有 3 个顶点要绘制。
        // instanceCount：用于实例渲染，1如果您不这样做，请使用。
        // firstVertex：用作顶点缓冲区的偏移量，定义 的最小值gl_VertexIndex。
        // firstInstance：用作实例渲染的偏移量，定义 的最小值gl_InstanceIndex。
//        commandBuffer.draw(mVertices.size(), 1, 0, 0);
        commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, mPipelineLayout, 0, {mDescriptorSets[mCurrentFrame]}, nullptr);
        commandBuffer.drawIndexed(mIndices.size(), 1, 0, 0, 0);
    }
    commandBuffer.endRenderPass();
    commandBuffer.end();
}


void TriangleTest::drawFrame() {
    vk::Result result = mDevice.waitForFences(1, &mInFlightFences[mCurrentFrame], vk::True, std::numeric_limits<uint64_t>::max());
    if (result != vk::Result::eSuccess) {
        throw std::runtime_error("waitForFences failed");
    }

//    std::cout << "acquireNextImageKHR" << std::endl;
    auto [acquireResult, imageIndex] = mDevice.acquireNextImageKHR(mSwapChain, std::numeric_limits<uint64_t>::max(), mImageAvailableSemaphores[mCurrentFrame]);
//    std::cout << "acquireNextImageKHR, acquireResult:" << acquireResult << " imageIndex:" << imageIndex << std::endl;
    // VK_ERROR_OUT_OF_DATE_KHR：交换链已与表面不兼容，不能再用于渲染。通常在窗口大小调整后发生。
    // VK_SUBOPTIMAL_KHR：交换链仍然可以成功显示到表面，但表面属性不再完全匹配。
    if (acquireResult == vk::Result::eErrorOutOfDateKHR) {
        std::cout << "acquireNextImageKHR: eErrorOutOfDateKHR, recreateSwapChain" << std::endl;
        recreateSwapChain();
        return;
    } else if (acquireResult != vk::Result::eSuccess && acquireResult != vk::Result::eSuboptimalKHR) {
        std::cout << "acquireNextImageKHR: failed" << acquireResult << std::endl;
        throw std::runtime_error("acquireNextImageKHR failed");
    }
//    std::cout << "acquireNextImageKHR eSuccess" << std::endl;

    mCommandBuffers[mCurrentFrame].reset();
    recordCommandBuffer(mCommandBuffers[mCurrentFrame], imageIndex);

    updateUniformBuffer(mCurrentFrame);

    vk::SubmitInfo submitInfo{};

    vk::Semaphore waitSemaphores[] = {mImageAvailableSemaphores[mCurrentFrame]};
    vk::PipelineStageFlags waitStages[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
    submitInfo.setWaitSemaphoreCount(1);
    submitInfo.setPWaitSemaphores(waitSemaphores);
    submitInfo.setPWaitDstStageMask(waitStages);
    submitInfo.setCommandBufferCount(1);
    submitInfo.setPCommandBuffers(&mCommandBuffers[mCurrentFrame]);

    vk::Semaphore signalSemaphores[] = {mRenderFinishedSemaphores[mCurrentFrame]};
    submitInfo.setSignalSemaphoreCount(1);
    submitInfo.setPSignalSemaphores(signalSemaphores);

    // 仅在我们提交工作时重置栅栏
    result = mDevice.resetFences(1, &mInFlightFences[mCurrentFrame]);
    if (result != vk::Result::eSuccess) {
        throw std::runtime_error("resetFences failed");
    }
    result = mGraphicsQueue.submit(1, &submitInfo, mInFlightFences[mCurrentFrame]);
    if (result != vk::Result::eSuccess) {
        throw std::runtime_error("graphicQueue.submit failed");
    }

    vk::PresentInfoKHR presentInfo{};
    presentInfo.setWaitSemaphores(signalSemaphores);
    presentInfo.setSwapchainCount(1);
    presentInfo.setPSwapchains(&mSwapChain);
    presentInfo.setImageIndices(imageIndex);

//    std::cout << "presentKHR, mFrameBufferResized:" << mFrameBufferResized << std::endl;

    // https://github.com/KhronosGroup/Vulkan-Hpp/issues/599
    // 当出现图片不匹配时， cpp风格的 presentKHR 会抛出异常， 而不是返回 result， 而C风格的 presentKHR 接口会返回 result
    try {
        result = mPresentQueue.presentKHR(presentInfo);
    } catch (const vk::OutOfDateKHRError &e) {
        std::cout << "mPresentQueue.presentKHR => OutOfDateKHRError" << std::endl;
        result = vk::Result::eErrorOutOfDateKHR;
    }

//    std::cout << "presentKHR result:" << result << std::endl;
    if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR || mFrameBufferResized) {
        mFrameBufferResized = false;
        std::cout << "presentKHR: eErrorOutOfDateKHR or eSuboptimalKHR or mFrameBufferResized, recreateSwapChain" << std::endl;
        recreateSwapChain();
        return;
    } else if (result != vk::Result::eSuccess) {
        throw std::runtime_error("presentKHR failed");
    }
//    std::cout << "presentKHR eSuccess" << std::endl;

    mCurrentFrame = (mCurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void TriangleTest::cleanFrameBuffers() {
    for (const auto &swapChainFrameBuffer: mSwapChainFrameBuffers) {
        mDevice.destroy(swapChainFrameBuffer);
    }

//    std::for_each(mSwapChainFrameBuffers.begin(), mSwapChainFrameBuffers.end(),[&](vk::Framebuffer& buffer){
//        mDevice.destroy(buffer);
//    });
}

void TriangleTest::createSyncObjects() {
    vk::SemaphoreCreateInfo semaphoreCreateInfo;

    vk::FenceCreateInfo fenceCreateInfo;
    // 已发出信号的状态下创建栅栏，以便第一次调用 vkWaitForFences()立即返回
    fenceCreateInfo.flags = vk::FenceCreateFlagBits::eSignaled;

    mImageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    mRenderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    mInFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        mImageAvailableSemaphores[i] = mDevice.createSemaphore(semaphoreCreateInfo);
        mRenderFinishedSemaphores[i] = mDevice.createSemaphore(semaphoreCreateInfo);
        mInFlightFences[i] = mDevice.createFence(fenceCreateInfo);
    }
}

void TriangleTest::cleanSyncObjects() {
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        mDevice.destroy(mImageAvailableSemaphores[i]);
        mDevice.destroy(mRenderFinishedSemaphores[i]);
        mDevice.destroy(mInFlightFences[i]);
    }
}

void TriangleTest::recreateSwapChain() {
    // 处理最小化
    int width;
    int height;
    glfwGetFramebufferSize(mWindow, &width, &height);
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(mWindow, &width, &height);
        glfwWaitEvents();
    }

    mDevice.waitIdle();

    cleanUpSwapChain();

    createSwapChain();
    createImageViews();
    createColorResources();
    createDepthResources();
    createFrameBuffers();
}

void TriangleTest::cleanUpSwapChain() {
    cleanFrameBuffers();
    cleanColorResources();
    cleanDepthResources();
    cleanImageViews();
    mDevice.destroy(mSwapChain);
}

void TriangleTest::createVertexBuffer() {
    vk::DeviceSize bufferSize = sizeof(mVertices[0]) * mVertices.size();

    auto [stagingBuffer, stagingBufferMemory] = createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc,
                                                             vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
    void *data = mDevice.mapMemory(stagingBufferMemory, 0, bufferSize, static_cast<vk::MemoryMapFlags>(0)/*vk::MemoryMapFlagBits::ePlacedEXT*/);
    {
        memcpy(data, mVertices.data(), (size_t) bufferSize);
    }
    mDevice.unmapMemory(stagingBufferMemory);

    std::tie(mVertexBuffer, mVertexBufferMemory) = createBuffer(bufferSize,
                                                                vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
                                                                vk::MemoryPropertyFlagBits::eDeviceLocal);

    copyBuffer(stagingBuffer, mVertexBuffer, bufferSize);

    mDevice.destroy(stagingBuffer);
    mDevice.freeMemory(stagingBufferMemory);
}

void TriangleTest::createIndexBuffer() {
    vk::DeviceSize bufferSize = sizeof(mIndices[0]) * mIndices.size();

    auto [stagingBuffer, stagingBufferMemory] = createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc,
                                                             vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
    void *data = mDevice.mapMemory(stagingBufferMemory, 0, bufferSize, static_cast<vk::MemoryMapFlags>(0)/*vk::MemoryMapFlagBits::ePlacedEXT*/);
    {
        memcpy(data, mIndices.data(), (size_t) bufferSize);
    }
    mDevice.unmapMemory(stagingBufferMemory);

    std::tie(mIndexBuffer, mIndexBufferMemory) = createBuffer(bufferSize,
                                                              vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer,
                                                              vk::MemoryPropertyFlagBits::eDeviceLocal);

    copyBuffer(stagingBuffer, mIndexBuffer, bufferSize);

    mDevice.destroy(stagingBuffer);
    mDevice.freeMemory(stagingBufferMemory);
}

uint32_t TriangleTest::findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties) {
    vk::PhysicalDeviceMemoryProperties memoryProperties = mPhysicalDevice.getMemoryProperties();

    for (int i = 0; i < memoryProperties.memoryTypeCount; i++) {
        if (typeFilter & (1 << i) && ((memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)) {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type !");
}

std::pair<vk::Buffer, vk::DeviceMemory> TriangleTest::createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties) {
    vk::BufferCreateInfo bufferCreateInfo{};
    bufferCreateInfo.setSize(size)
            .setUsage(usage)
            .setSharingMode(vk::SharingMode::eExclusive);

    vk::Buffer buffer = mDevice.createBuffer(bufferCreateInfo);
    vk::MemoryRequirements memoryRequirements = mDevice.getBufferMemoryRequirements(buffer);

    uint32_t memoryType = findMemoryType(memoryRequirements.memoryTypeBits, properties);
    vk::MemoryAllocateInfo memoryAllocateInfo{};
    memoryAllocateInfo.setAllocationSize(memoryRequirements.size)
            .setMemoryTypeIndex(memoryType);

    vk::DeviceMemory bufferMemory = mDevice.allocateMemory(memoryAllocateInfo);
    mDevice.bindBufferMemory(buffer, bufferMemory, 0);

    return {buffer, bufferMemory};
}

void TriangleTest::copyBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size) {
    vk::CommandBuffer commandBuffer = beginSingleTimeCommands();
    {
        vk::BufferCopy bufferCopy;
        bufferCopy.setSrcOffset(0)
                .setDstOffset(0)
                .setSize(size);

        commandBuffer.copyBuffer(srcBuffer, dstBuffer, bufferCopy);
    }
    endSingleTimeCommands(commandBuffer);
}

void TriangleTest::createDescriptorSetLayout() {
    vk::DescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.setBinding(0)
            .setDescriptorType(vk::DescriptorType::eUniformBuffer)
                    // mvp 保存在一个 uniform 对象中， 描述符描述的 shader 的变量可以表示 unifrom 数组， 所以用长度为 1 的数组表示单个 uniform
            .setDescriptorCount(1)
                    // 在 vertex shader 中使用
            .setStageFlags(vk::ShaderStageFlagBits::eVertex)
                    // pImmutableSamplers 字段仅与图像采样相关的描述符相关
            .setPImmutableSamplers(nullptr);

    vk::DescriptorSetLayoutBinding samplerLayoutBinding{};
    samplerLayoutBinding.setBinding(1)
            .setDescriptorCount(1)
            .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
            .setPImmutableSamplers(nullptr)
            .setStageFlags(vk::ShaderStageFlagBits::eFragment);

    std::array<vk::DescriptorSetLayoutBinding, 2> bindings = {uboLayoutBinding, samplerLayoutBinding};

    vk::DescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{};
    descriptorSetLayoutCreateInfo
//            .setBindingCount(bindings.size())
            .setBindings(bindings);

    mDescriptorSetLayout = mDevice.createDescriptorSetLayout(descriptorSetLayoutCreateInfo);
}

void TriangleTest::createUniformBuffers() {
    vk::DeviceSize bufferSize = sizeof(UniformBufferObject);

    mUniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    mUniformBufferMemories.resize(MAX_FRAMES_IN_FLIGHT);
    mUniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        std::tie(mUniformBuffers[i], mUniformBufferMemories[i]) = createBuffer(bufferSize,
                                                                               vk::BufferUsageFlagBits::eUniformBuffer,
                                                                               vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
        mUniformBuffersMapped[i] = mDevice.mapMemory(mUniformBufferMemories[i], 0, bufferSize, static_cast<vk::MemoryMapFlags>(0));
    }
}

void TriangleTest::cleanUniformBuffers() {
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        mDevice.destroy(mUniformBuffers[i]);
        mDevice.freeMemory(mUniformBufferMemories[i]);
    }
}

void TriangleTest::updateUniformBuffer(uint32_t frameIndex) {
    static auto startTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    UniformBufferObject ubo{};
    float scale = 1.0f;
    glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(scale, scale, scale));
    model = glm::rotate(model, time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.model = model;
    ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f),
                           glm::vec3(0.0f, 0.0f, 0.0f),
                           glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.proj = glm::perspective(glm::radians(45.0f), (float) mSwapChainExtent.width / (float) mSwapChainExtent.height, 0.1f, 10.0f);
    // GLM 最初为 OpenGL 设计，OpenGL 中剪裁坐标的Y坐标是反转的。补偿这种情况的最简单方法是翻转投影矩阵中Y轴缩放因子的符号。如果不这样做，图像将上下颠倒。
    ubo.proj[1][1] *= -1;

    memcpy(mUniformBuffersMapped[frameIndex], &ubo, sizeof(ubo));
}

void TriangleTest::createDescriptorPool() {
    vk::DescriptorPoolSize uboPoolSize{};
    uboPoolSize.setType(vk::DescriptorType::eUniformBuffer)
            .setDescriptorCount(MAX_FRAMES_IN_FLIGHT);

    vk::DescriptorPoolSize samplerPoolSize{};
    samplerPoolSize.setType(vk::DescriptorType::eCombinedImageSampler)
            .setDescriptorCount(MAX_FRAMES_IN_FLIGHT);

    std::array<vk::DescriptorPoolSize, 2> poolSizes = {uboPoolSize, samplerPoolSize};

    vk::DescriptorPoolCreateInfo descriptorPoolCreateInfo;
    descriptorPoolCreateInfo
            .setPoolSizeCount(poolSizes.size())
            .setPoolSizes(poolSizes)
            .setMaxSets(MAX_FRAMES_IN_FLIGHT)
//            .setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet);
            .setFlags(static_cast<vk::DescriptorPoolCreateFlags>(0));

    mDescriptorPool = mDevice.createDescriptorPool(descriptorPoolCreateInfo);
}

void TriangleTest::createDescriptorSets() {
    std::vector<vk::DescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, mDescriptorSetLayout);
    vk::DescriptorSetAllocateInfo allocateInfo{};
    allocateInfo.setDescriptorPool(mDescriptorPool)
            .setDescriptorSetCount(MAX_FRAMES_IN_FLIGHT)
            .setPSetLayouts(layouts.data());

//    mDescriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
    mDescriptorSets = mDevice.allocateDescriptorSets(allocateInfo);

    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vk::DescriptorBufferInfo uboDescriptorBufferInfo{};
        uboDescriptorBufferInfo.setBuffer(mUniformBuffers[i])
                .setOffset(0)
                .setRange(sizeof(UniformBufferObject));

        vk::WriteDescriptorSet uboWriteDescriptorSet{};
        uboWriteDescriptorSet.setDstSet(mDescriptorSets[i])
                .setDstBinding(0)
                .setDstArrayElement(0)
                .setDescriptorType(vk::DescriptorType::eUniformBuffer)
                .setDescriptorCount(1)
                        // 下面根据 DescriptorType 3 选 1 设置， 这里的描述符基于缓冲区，所以选择使用 pBufferInfo。
                        // pBufferInfo 字段用于引用缓冲区数据的描述符
                .setPBufferInfo(&uboDescriptorBufferInfo)
                        // pImageInfo 用于引用图像数据的描述符
                .setPImageInfo(nullptr)
                        // pTexelBufferView 用于引用缓冲区视图的描述符
                .setPTexelBufferView(nullptr);


        vk::DescriptorImageInfo samplerDescriptorImageInfo;
        samplerDescriptorImageInfo.setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
                .setImageView(mTextureImageView)
                .setSampler(mTextureSampler);

        vk::WriteDescriptorSet samplerWriteDescriptorSet{};
        samplerWriteDescriptorSet.setDstSet(mDescriptorSets[i])
                .setDstBinding(1)
                .setDstArrayElement(0)
                .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
                .setDescriptorCount(1)
                        // 下面根据 DescriptorType 3 选 1 设置， 这里的描述符基于缓冲区，所以选择使用 pBufferInfo。
                        // pBufferInfo 字段用于引用缓冲区数据的描述符
                .setPBufferInfo(nullptr)
                        // pImageInfo 用于引用图像数据的描述符
                .setPImageInfo(&samplerDescriptorImageInfo)
                        // pTexelBufferView 用于引用缓冲区视图的描述符
                .setPTexelBufferView(nullptr);

        std::array<vk::WriteDescriptorSet, 2> writeDescriptorSets = {uboWriteDescriptorSet, samplerWriteDescriptorSet};
        // descriptorCopies 用于相互复制描述符
        mDevice.updateDescriptorSets(writeDescriptorSets, nullptr);
    }
}

void TriangleTest::createTextureImage() {
    std::cout << "createTextureImage" << std::endl;

    int textureWidth;
    int textureHeight;
    int textureChannelCount;

    stbi_uc *pixels = stbi_load(TEXTURE_PATH, &textureWidth, &textureHeight, &textureChannelCount, STBI_rgb_alpha);
    vk::DeviceSize imageSize = textureWidth * textureHeight * 4;
    mMipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(textureWidth, textureHeight)))) + 1;

    if (pixels == nullptr) {
        throw std::runtime_error("failed to load texture image");
    }

    auto [stagingBuffer, stagingBufferMemory] = createBuffer(imageSize, vk::BufferUsageFlagBits::eTransferSrc,
                                                             vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
    void *data = mDevice.mapMemory(stagingBufferMemory, 0, imageSize, static_cast<vk::MemoryMapFlags>(0));
    {
        memcpy(data, pixels, imageSize);
    }
    mDevice.unmapMemory(stagingBufferMemory);
    stbi_image_free(pixels);

    std::tie(mTextureImage, mTextureImageMemory) = createImage(textureWidth, textureHeight, mMipLevels, vk::SampleCountFlagBits::e1,
                                                               vk::Format::eR8G8B8A8Srgb, vk::ImageTiling::eOptimal,
                                                               vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
                                                               vk::MemoryPropertyFlagBits::eDeviceLocal);

    transitionImageLayout(mTextureImage, vk::Format::eR8G8B8A8Srgb, vk::ImageLayout::eUndefined,
                          vk::ImageLayout::eTransferDstOptimal, mMipLevels);
    {
        copyBufferToImage(stagingBuffer, mTextureImage, textureWidth, textureHeight);
    }
//    transitionImageLayout(mTextureImage, vk::Format::eR8G8B8A8Srgb, vk::ImageLayout::eTransferDstOptimal,
//                          vk::ImageLayout::eShaderReadOnlyOptimal, mMipLevels);
    generateMipmaps(mTextureImage, vk::Format::eR8G8B8A8Srgb, textureWidth, textureHeight, mMipLevels);

    mDevice.destroy(stagingBuffer);
    mDevice.freeMemory(stagingBufferMemory);
}

std::pair<vk::Image, vk::DeviceMemory>
TriangleTest::createImage(uint32_t width, uint32_t height, uint32_t mipLevels, vk::SampleCountFlagBits numSamples,
                          vk::Format format, vk::ImageTiling imageTiling, vk::ImageUsageFlags imageUsage, vk::MemoryPropertyFlags memoryProperty) {
//    vk::DeviceSize imageSize = textureWidth * textureHeight * 4;

    vk::Extent3D extent;
    extent.setWidth(width)
            .setHeight(height)
            .setDepth(1);
    vk::ImageCreateInfo imageCreateInfo;
    imageCreateInfo.setImageType(vk::ImageType::e2D)
            .setExtent(extent)
            .setMipLevels(mipLevels)
            .setArrayLayers(1)
            .setFormat(format)
            .setTiling(imageTiling)
            .setInitialLayout(vk::ImageLayout::eUndefined)
            .setUsage(imageUsage)
            .setSharingMode(vk::SharingMode::eExclusive)
            .setSamples(numSamples)
            .setFlags(static_cast<vk::ImageCreateFlags>(0));

    vk::Image image = mDevice.createImage(imageCreateInfo);

    vk::MemoryRequirements memoryRequirements = mDevice.getImageMemoryRequirements(image);

    vk::MemoryAllocateInfo memoryAllocateInfo;
    uint32_t memoryType = findMemoryType(memoryRequirements.memoryTypeBits, memoryProperty);
    memoryAllocateInfo.setAllocationSize(memoryRequirements.size)
            .setMemoryTypeIndex(memoryType);

    vk::DeviceMemory imageMemory = mDevice.allocateMemory(memoryAllocateInfo);

    mDevice.bindImageMemory(image, imageMemory, 0);

    return {image, imageMemory};
}

vk::CommandBuffer TriangleTest::beginSingleTimeCommands() {
    vk::CommandBufferAllocateInfo commandBufferAllocateInfo{};
    commandBufferAllocateInfo.setLevel(vk::CommandBufferLevel::ePrimary)
            .setCommandPool(mCommandPool)
            .setCommandBufferCount(1);

    vk::CommandBuffer commandBuffer = mDevice.allocateCommandBuffers(commandBufferAllocateInfo)[0];

    vk::CommandBufferBeginInfo commandBufferBeginInfo{};
    commandBufferBeginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    commandBuffer.begin(commandBufferBeginInfo);

    return commandBuffer;
}

void TriangleTest::endSingleTimeCommands(vk::CommandBuffer &commandBuffer) {
    commandBuffer.end();

    vk::SubmitInfo submitInfo{};
    submitInfo.setCommandBufferCount(1)
            .setPCommandBuffers(&commandBuffer);

    mGraphicsQueue.submit(submitInfo);
    mGraphicsQueue.waitIdle();

    mDevice.freeCommandBuffers(mCommandPool, commandBuffer);
}

void TriangleTest::transitionImageLayout(vk::Image image, vk::Format format, vk::ImageLayout oldImageLayout, vk::ImageLayout newImageLayout, uint32_t mipLevels) {
    vk::CommandBuffer commandBuffer = beginSingleTimeCommands();
    {
        vk::ImageSubresourceRange imageSubresourceRange;
        imageSubresourceRange
                .setBaseMipLevel(0)
                .setLevelCount(mipLevels)
                .setBaseArrayLayer(0)
                .setLayerCount(1);

        // 注意这里一定要是 vk::ImageLayout::eDepthStencilAttachmentOptimal ， 写成了 vk::ImageLayout::eStencilAttachmentOptimal 后面会报警告
        if (newImageLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal) {
            if (hasStencilComponent(format)) {
                imageSubresourceRange.setAspectMask(vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil);
            } else {
                imageSubresourceRange.setAspectMask(vk::ImageAspectFlagBits::eDepth);
            }
        } else {
            imageSubresourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor);
        }


        vk::ImageMemoryBarrier imageMemoryBarrier;
        imageMemoryBarrier.setOldLayout(oldImageLayout)
                .setNewLayout(newImageLayout)
                .setSrcQueueFamilyIndex(vk::QueueFamilyIgnored)
                .setDstQueueFamilyIndex(vk::QueueFamilyIgnored)
                .setImage(image)
                .setSubresourceRange(imageSubresourceRange);
//                .setSrcAccessMask(static_cast<vk::AccessFlags>(0))
//                .setDstAccessMask(static_cast<vk::AccessFlags>(0));

        vk::PipelineStageFlags sourceStage;
        vk::PipelineStageFlags destinationStage;

        if (oldImageLayout == vk::ImageLayout::eUndefined && newImageLayout == vk::ImageLayout::eTransferDstOptimal) {
            imageMemoryBarrier.setSrcAccessMask(static_cast<vk::AccessFlags>(0))
                    .setDstAccessMask(vk::AccessFlagBits::eTransferWrite);

            sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
            destinationStage = vk::PipelineStageFlagBits::eTransfer;
        } else if (oldImageLayout == vk::ImageLayout::eTransferDstOptimal && newImageLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {
            imageMemoryBarrier.setSrcAccessMask(vk::AccessFlagBits::eTransferWrite)
                    .setDstAccessMask(vk::AccessFlagBits::eShaderRead);

            sourceStage = vk::PipelineStageFlagBits::eTransfer;
            destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
        } else if (oldImageLayout == vk::ImageLayout::eUndefined && newImageLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal) {
            imageMemoryBarrier.setSrcAccessMask(static_cast<vk::AccessFlags>(0))
                    .setDstAccessMask(vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite);

            sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
            destinationStage = vk::PipelineStageFlagBits::eEarlyFragmentTests;
        } else {
            throw std::runtime_error("unsupported layout transition!");
        }

        std::cout << "pipelineBarrier" << std::endl;
        commandBuffer.pipelineBarrier(sourceStage,
                                      destinationStage,
                                      static_cast<vk::DependencyFlags>(0),
                // 内存屏障
                                      {},
                // 缓冲区内存屏障
                                      {},
                // 图像内存屏障
                                      {imageMemoryBarrier});


    }
    endSingleTimeCommands(commandBuffer);
}

void TriangleTest::copyBufferToImage(vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height) {
    vk::CommandBuffer commandBuffer = beginSingleTimeCommands();

    vk::ImageSubresourceLayers imageSubresourceLayers;
    imageSubresourceLayers.setAspectMask(vk::ImageAspectFlagBits::eColor)
            .setMipLevel(0)
            .setBaseArrayLayer(0)
            .setLayerCount(1);

    vk::Offset3D offset{0, 0, 0};
    vk::Extent3D extent{width, height, 1};

    vk::BufferImageCopy bufferImageCopy;
    bufferImageCopy.setBufferOffset(0)
            .setBufferRowLength(0)
            .setBufferImageHeight(0)
            .setImageSubresource(imageSubresourceLayers)
            .setImageOffset(offset)
            .setImageExtent(extent);

    commandBuffer.copyBufferToImage(buffer, image, vk::ImageLayout::eTransferDstOptimal, {bufferImageCopy});

    endSingleTimeCommands(commandBuffer);
}

void TriangleTest::createTextureImageView() {
    mTextureImageView = createImageView(mTextureImage, vk::Format::eR8G8B8A8Srgb, vk::ImageAspectFlagBits::eColor, mMipLevels);
}

vk::ImageView TriangleTest::createImageView(const vk::Image &image, vk::Format format, vk::ImageAspectFlags imageAspect, uint32_t mipLevels) {
    vk::ImageViewCreateInfo imageViewCreateInfo;

    imageViewCreateInfo.setImage(image)
            .setViewType(vk::ImageViewType::e2D)
            .setFormat(format);
//            .setSubresourceRange(imageSubresourceRange)
//            .setComponents(componentMapping);

    vk::ImageSubresourceRange &imageSubresourceRange = imageViewCreateInfo.subresourceRange;
    imageSubresourceRange.setAspectMask(imageAspect)
            .setBaseMipLevel(0)
            .setLevelCount(mipLevels)
            .setBaseArrayLayer(0)
            .setLayerCount(1);

    vk::ComponentMapping &componentMapping = imageViewCreateInfo.components;
    componentMapping.setR(vk::ComponentSwizzle::eIdentity)
            .setG(vk::ComponentSwizzle::eIdentity)
            .setB(vk::ComponentSwizzle::eIdentity)
            .setA(vk::ComponentSwizzle::eIdentity);

    return mDevice.createImageView(imageViewCreateInfo);
}

void TriangleTest::createTextureSampler() {
    vk::PhysicalDeviceProperties properties = mPhysicalDevice.getProperties();

    vk::SamplerCreateInfo samplerCreateInfo;
    samplerCreateInfo.setMagFilter(vk::Filter::eLinear)
            .setMinFilter(vk::Filter::eLinear)
            .setAddressModeU(vk::SamplerAddressMode::eRepeat)
            .setAddressModeV(vk::SamplerAddressMode::eRepeat)
            .setAddressModeW(vk::SamplerAddressMode::eRepeat)
            .setAnisotropyEnable(vk::True)
            .setMaxAnisotropy(properties.limits.maxSamplerAnisotropy)
            .setBorderColor(vk::BorderColor::eIntOpaqueBlack)
                    // 是否使用不归一化的坐标(x:[0~width], y:[0~height])， 通常使用归一化坐标(x:[0~1.0], y:[0~1.0])， 所以设置为 false，
            .setUnnormalizedCoordinates(vk::False)
            .setCompareEnable(vk::False)
            .setCompareOp(vk::CompareOp::eAlways)
            .setMipmapMode(vk::SamplerMipmapMode::eLinear)
            .setMipLodBias(0.0f)
            .setMinLod(0.0f)
                    // 测试： 强制使用高等级 mipmap （更模糊）
//            .setMinLod(static_cast<float >(mMipLevels / 2))
            .setMaxLod(static_cast<float >(mMipLevels));

    mTextureSampler = mDevice.createSampler(samplerCreateInfo);
}

void TriangleTest::createDepthResources() {
    std::cout << "createDepthResources" << std::endl;
    std::vector<vk::Format> candidate;
    vk::ImageTiling imageTiling;
    vk::FormatFeatureFlags formatFeatureFlags;

    vk::Format depthFormat = findDepthFormat();

    std::tie(mDepthImage, mDepthDeviceMemory) = createImage(mSwapChainExtent.width, mSwapChainExtent.height, 1, mMsaaSamples,
                                                            depthFormat,
                                                            vk::ImageTiling::eOptimal,
                                                            vk::ImageUsageFlagBits::eDepthStencilAttachment,
                                                            vk::MemoryPropertyFlagBits::eDeviceLocal);
    mDepthImageView = createImageView(mDepthImage, depthFormat, vk::ImageAspectFlagBits::eDepth, 1);
    transitionImageLayout(mDepthImage, depthFormat, vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthStencilAttachmentOptimal, 1);
}

vk::Format TriangleTest::findSupportedFormat(const std::vector<vk::Format> &candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features) {
    for (const auto &format: candidates) {
        vk::FormatProperties properties = mPhysicalDevice.getFormatProperties(format);
        if (tiling == vk::ImageTiling::eLinear && (properties.linearTilingFeatures & features) == features) {
            return format;
        } else if (tiling == vk::ImageTiling::eOptimal && (properties.optimalTilingFeatures & features) == features) {
            return format;
        }
    }

    throw std::runtime_error("failed to find supported format !");
}

vk::Format TriangleTest::findDepthFormat() {
    return findSupportedFormat(
            {vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint},
            vk::ImageTiling::eOptimal,
            vk::FormatFeatureFlagBits::eDepthStencilAttachment
    );
}

bool TriangleTest::hasStencilComponent(vk::Format format) {
    return format == vk::Format::eD32SfloatS8Uint || format == vk::Format::eD24UnormS8Uint;
}

void TriangleTest::cleanDepthResources() {
    mDevice.destroy(mDepthImage);
    mDevice.destroy(mDepthImageView);
    mDevice.freeMemory(mDepthDeviceMemory);
}

void TriangleTest::loadModel() {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warning;
    std::string error;

    bool success = tinyobj::LoadObj(&attrib, &shapes, &materials, &warning, &error, MODEL_PATH);
    if (!success) {
        throw std::runtime_error(warning + error);
    }

    std::unordered_map<Vertex, uint32_t> uniqueVertices{};

    for (const auto &shape: shapes) {
        for (const auto &index: shape.mesh.indices) {
            Vertex vertex{};

            vertex.pos = {
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2]
            };

            vertex.texCoord = {
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    1.0f - attrib.texcoords[2 * index.texcoord_index + 1],
            };

            vertex.color = {1.0f, 1.0f, 1.0f};

            if (uniqueVertices.count(vertex) == 0) {
                uniqueVertices[vertex] = mVertices.size();
                mVertices.push_back(vertex);
            }

            mIndices.push_back(uniqueVertices[vertex]);
        }
    }
}

void TriangleTest::generateMipmaps(vk::Image image, vk::Format imageFormat, int textureWidth, int textureHeight, uint32_t mipLevels) {
    vk::FormatProperties formatProperties = mPhysicalDevice.getFormatProperties(imageFormat);
    if (!(formatProperties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eSampledImageFilterLinear)) {
        throw std::runtime_error("texture image format does not support linear blitting!");
    }


    vk::CommandBuffer commandBuffer = beginSingleTimeCommands();
    {
        vk::ImageMemoryBarrier imageMemoryBarrier;
        imageMemoryBarrier.setImage(image)
                .setSrcQueueFamilyIndex(vk::QueueFamilyIgnored)
                .setDstQueueFamilyIndex(vk::QueueFamilyIgnored);

        vk::ImageSubresourceRange &subresourceRange = imageMemoryBarrier.subresourceRange;
        subresourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor)
                .setBaseArrayLayer(0)
                .setLayerCount(1)
                .setLevelCount(1);

        uint32_t mipWidth = textureWidth;
        uint32_t mipHeight = textureHeight;

        for (int i = 1; i < mipLevels; i++) {
            subresourceRange.setBaseMipLevel(i - 1);
            imageMemoryBarrier.setOldLayout(vk::ImageLayout::eTransferDstOptimal)
                    .setNewLayout(vk::ImageLayout::eTransferSrcOptimal)
                    .setSrcAccessMask(vk::AccessFlagBits::eTransferWrite)
                    .setDstAccessMask(vk::AccessFlagBits::eTransferRead);

            commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTransfer,
                                          static_cast<vk::DependencyFlags>(0),
                                          {},
                                          {},
                                          {imageMemoryBarrier});

            vk::ImageBlit imageBlit;

            std::array<vk::Offset3D, 2> &srcOffsets = imageBlit.srcOffsets;
            srcOffsets[0] = vk::Offset3D{0, 0, 0};
            srcOffsets[1] = vk::Offset3D{static_cast<int32_t>(mipWidth), static_cast<int32_t>(mipHeight), 1};

            vk::ImageSubresourceLayers &srcImageSubresourceLayers = imageBlit.srcSubresource;
            srcImageSubresourceLayers.setAspectMask(vk::ImageAspectFlagBits::eColor)
                    .setMipLevel(i - 1)
                    .setBaseArrayLayer(0)
                    .setLayerCount(1);

            std::array<vk::Offset3D, 2> &dstOffsets = imageBlit.dstOffsets;
            dstOffsets[0] = vk::Offset3D{0, 0, 0};
            dstOffsets[1] = vk::Offset3D{static_cast<int32_t>(mipWidth > 1 ? mipWidth / 2 : 1), static_cast<int32_t>(mipHeight > 1 ? mipHeight / 2 : 1), 1};

            vk::ImageSubresourceLayers &dstImageSubresourceLayers = imageBlit.dstSubresource;
            dstImageSubresourceLayers.setAspectMask(vk::ImageAspectFlagBits::eColor)
                    .setMipLevel(i)
                    .setBaseArrayLayer(0)
                    .setLayerCount(1);

            commandBuffer.blitImage(image, vk::ImageLayout::eTransferSrcOptimal, image, vk::ImageLayout::eTransferDstOptimal, {imageBlit}, vk::Filter::eLinear);

            imageMemoryBarrier.setOldLayout(vk::ImageLayout::eTransferSrcOptimal)
                    .setNewLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
                    .setSrcAccessMask(vk::AccessFlagBits::eTransferWrite)
                    .setDstAccessMask(vk::AccessFlagBits::eShaderRead);

            commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader,
                                          static_cast<vk::DependencyFlags>(0),
                                          {},
                                          {},
                                          {imageMemoryBarrier});

            if (mipWidth > 1) {
                mipWidth /= 2;
            }

            if (mipHeight > 1) {
                mipHeight /= 2;
            }
        }

        subresourceRange.setBaseMipLevel(mipLevels - 1);
        imageMemoryBarrier.setOldLayout(vk::ImageLayout::eTransferDstOptimal)
                .setNewLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
                .setSrcAccessMask(vk::AccessFlagBits::eTransferWrite)
                .setDstAccessMask(vk::AccessFlagBits::eShaderRead);

        commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader,
                                      static_cast<vk::DependencyFlags>(0),
                                      {},
                                      {},
                                      {imageMemoryBarrier});
    }
    endSingleTimeCommands(commandBuffer);
}

vk::SampleCountFlagBits TriangleTest::getMaxUsableSampleCount() {
    vk::PhysicalDeviceProperties properties = mPhysicalDevice.getProperties();
    vk::PhysicalDeviceLimits &limits = properties.limits;

    vk::SampleCountFlags counts = limits.framebufferColorSampleCounts & limits.framebufferDepthSampleCounts;
    if (counts & vk::SampleCountFlagBits::e64) {
        return vk::SampleCountFlagBits::e64;
    } else if (counts & vk::SampleCountFlagBits::e32) {
        return vk::SampleCountFlagBits::e32;
    } else if (counts & vk::SampleCountFlagBits::e16) {
        return vk::SampleCountFlagBits::e16;
    } else if (counts & vk::SampleCountFlagBits::e8) {
        return vk::SampleCountFlagBits::e8;
    } else if (counts & vk::SampleCountFlagBits::e4) {
        return vk::SampleCountFlagBits::e4;
    } else if (counts & vk::SampleCountFlagBits::e2) {
        return vk::SampleCountFlagBits::e2;
    } else {
        return vk::SampleCountFlagBits::e1;
    }
}

void TriangleTest::createColorResources() {
    vk::Format colorFormat = mSwapChainImageFormat.format;

    std::tie(mColorImage, mColorDeviceMemory) = createImage(mSwapChainExtent.width, mSwapChainExtent.height, 1, mMsaaSamples, colorFormat,
                                                            vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eTransientAttachment | vk::ImageUsageFlagBits::eColorAttachment,
                                                            vk::MemoryPropertyFlagBits::eDeviceLocal);
    mColorImageView = createImageView(mColorImage, colorFormat, vk::ImageAspectFlagBits::eColor, 1);
}

void TriangleTest::cleanColorResources() {
    mDevice.destroy(mColorImage);
    mDevice.destroy(mColorImageView);
    mDevice.freeMemory(mColorDeviceMemory);
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
    createInfo.messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eError
                                 | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning
                                 | vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo;

    createInfo.messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral
                             | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation
                             | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance
                             | vk::DebugUtilsMessageTypeFlagBitsEXT::eDeviceAddressBinding;
    createInfo.pfnUserCallback = debugCallback;
}