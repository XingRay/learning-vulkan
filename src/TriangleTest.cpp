//
// Created by leixing on 2024/5/12.
//

#include "TriangleTest.h"
#include "vulkan/vulkan.hpp"

#include <iostream>
#include <map>
#include <set>
#include <string>

#include "QueueFamilyIndices.h"
#include "FileUtil.h"


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
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    // The first three parameters specify the width, height and title of the mWindow.
    // The fourth parameter allows you to optionally specify a monitor to open the mWindow on
    // and the last parameter is only relevant to OpenGL.
    mWindow = glfwCreateWindow(mWidth, mHeight, "triangle test", nullptr, nullptr);
}

void TriangleTest::initVulkan() {
    createInstance();
    setupDebugMessenger();
    createSurface();
    pickPhysicalDevice();
    createLogicDevice();
    createSwapChain();
    createImageViews();
    createRenderPass();
    createGraphicsPipeline();
    createFrameBuffers();
    createCommandPool();
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

    cleanSyncObjects();
    mDevice.destroy(mCommandPool);
    cleanFrameBuffers();
    mDevice.destroy(mGraphicsPipeline);
    mDevice.destroy(mPipelineLayout);
    mDevice.destroy(mRenderPass);
    cleanImageViews();
    mDevice.destroy(mSwapChain);
    mDevice.destroy();

    mInstance.destroy(mSurface);
    if (mEnableValidationLayer) {
        destroyDebugUtilsMessengerExt(nullptr);
    }
    mInstance.destroy();

    glfwDestroyWindow(mWindow);
    glfwTerminate();
}

void TriangleTest::createInstance() {
    std::cout << "createInstance\n";

    vk::ApplicationInfo appInfo;
    appInfo.sType = vk::StructureType::eApplicationInfo;
//    appInfo.setPApplicationName("triangle test");
//    appInfo.setApplicationVersion(VK_MAKE_VERSION(1, 0, 0));
//    appInfo.setPEngineName("no engine");
//    appInfo.setEngineVersion(VK_MAKE_VERSION(1, 0, 0));
    appInfo.setApiVersion(VK_API_VERSION_1_3);

    vk::InstanceCreateInfo createInfo;
    createInfo.sType = vk::StructureType::eInstanceCreateInfo;
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
    std::cout << "createLogicDevice\n";
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
        queueCreateInfo.sType = vk::StructureType::eDeviceQueueCreateInfo;
        queueCreateInfo.queueFamilyIndex = graphicFamilyIndex;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;

        queueCreateInfos.push_back(queueCreateInfo);
    } else {
        std::array<uint32_t, 2> queueFamilyIndices = {graphicFamilyIndex, presentFamilyIndex};
        for (uint32_t queueFamilyIndex: queueFamilyIndices) {
            vk::DeviceQueueCreateInfo queueCreateInfo;
            queueCreateInfo.sType = vk::StructureType::eDeviceQueueCreateInfo;
            queueCreateInfo.queueFamilyIndex = queueFamilyIndex;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }
    }


    vk::PhysicalDeviceFeatures deviceFeatures{};

    vk::DeviceCreateInfo deviceCreateInfo;
    deviceCreateInfo.sType = vk::StructureType::eDeviceCreateInfo;
    deviceCreateInfo.setPQueueCreateInfos(queueCreateInfos.data());
    deviceCreateInfo.setQueueCreateInfoCount(queueCreateInfos.size());
    deviceCreateInfo.setPEnabledFeatures(&deviceFeatures);
    deviceCreateInfo.setEnabledExtensionCount(mRequiredExtensions.size());
    deviceCreateInfo.setPpEnabledExtensionNames(mRequiredExtensions.data());

    if (mEnableValidationLayer) {
        deviceCreateInfo.enabledLayerCount = mValidationLayers.size();
        deviceCreateInfo.setPpEnabledLayerNames(mValidationLayers.data());
    } else {
        deviceCreateInfo.enabledLayerCount = 0;
    }

    // 创建逻辑设备的同时会根据 deviceCreateInfo.pQueueCreateInfo 创建任务队列
    mDevice = mPhysicalDevice.createDevice(deviceCreateInfo);
    // 从逻辑设备中取出任务队列, 第二个参数为下标, 总共创建了一个队列,所以这里下标为 0
    mGraphicQueue = mDevice.getQueue(graphicFamilyIndex, 0);
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
    createInfo.sType = vk::StructureType::eSwapchainCreateInfoKHR;
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
        const auto &image = mSwapChainImages[i];

        vk::ImageViewCreateInfo createInfo;
        createInfo.sType = vk::StructureType::eImageViewCreateInfo;
        createInfo.image = image;
        createInfo.viewType = vk::ImageViewType::e2D;
        createInfo.format = mSwapChainImageFormat.format;

        createInfo.components.r = vk::ComponentSwizzle::eIdentity;
        createInfo.components.g = vk::ComponentSwizzle::eIdentity;
        createInfo.components.b = vk::ComponentSwizzle::eIdentity;
        createInfo.components.a = vk::ComponentSwizzle::eIdentity;

        createInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        // mipmap
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        // 多图层, 比如用于vr/3d, 左右眼各自有不同的图层, 这里在屏幕显示图片,只需要一个图层
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        mSwapChainImageViews[i] = mDevice.createImageView(createInfo);
    }
}

void TriangleTest::cleanImageViews() {
    for (auto imageView: mSwapChainImageViews) {
        mDevice.destroy(imageView);
    }
}

void TriangleTest::createRenderPass() {
    vk::AttachmentDescription colorAttachmentDescription;
    colorAttachmentDescription.setFormat(mSwapChainImageFormat.format);
    colorAttachmentDescription.setSamples(vk::SampleCountFlagBits::e1);

    //载入图像前将帧缓冲清0
    colorAttachmentDescription.setLoadOp(vk::AttachmentLoadOp::eClear);

    // 渲染图像之后将图像数据保存
    colorAttachmentDescription.setStoreOp(vk::AttachmentStoreOp::eStore);

    // 模版缓冲, 这里不关注
    colorAttachmentDescription.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare);
    colorAttachmentDescription.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare);

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
    colorAttachmentDescription.setInitialLayout(vk::ImageLayout::eUndefined);
    // 我们希望图像在渲染后准备好使用交换链进行呈现，所以我们设置finalLayout 为 VK_IMAGE_LAYOUT_PRESENT_SRC_KHRas
    colorAttachmentDescription.setFinalLayout(vk::ImageLayout::ePresentSrcKHR);

    vk::AttachmentReference colorAttachmentReference;
    // 多个 colorAttachmentDescription 组成数组, 上面只有一个 colorAttachmentDescription, 那么下标为 0
    colorAttachmentReference.attachment = 0;
    colorAttachmentReference.layout = vk::ImageLayout::eColorAttachmentOptimal;


    vk::SubpassDescription subpassDescription;
    subpassDescription.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics);
    subpassDescription.setColorAttachmentCount(1);
    subpassDescription.setPColorAttachments(&colorAttachmentReference);


    vk::SubpassDependency subpassDependency;
    subpassDependency.setSrcSubpass(vk::SubpassExternal);
    subpassDependency.setDstSubpass(0);
    subpassDependency.setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
    subpassDependency.setSrcAccessMask(vk::AccessFlagBits::eNone);
    subpassDependency.setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
    subpassDependency.setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite);


    vk::RenderPassCreateInfo renderPassCreateInfo;
    renderPassCreateInfo.sType = vk::StructureType::eRenderPassCreateInfo;
    renderPassCreateInfo.setAttachmentCount(1);
    renderPassCreateInfo.setPAttachments(&colorAttachmentDescription);
    renderPassCreateInfo.setSubpassCount(1);
    renderPassCreateInfo.setPSubpasses(&subpassDescription);
    renderPassCreateInfo.setDependencyCount(1);
    renderPassCreateInfo.setPDependencies(&subpassDependency);


    mRenderPass = mDevice.createRenderPass(renderPassCreateInfo);
}

void TriangleTest::createGraphicsPipeline() {
    auto vertexShaderCode = FileUtil::readFile("shader/vertex.spv");
    auto fragmentShaderCode = FileUtil::readFile("shader/fragment.spv");

    vk::ShaderModule vertexModule = createShaderModule(vertexShaderCode);
    vk::ShaderModule fragmentModule = createShaderModule(fragmentShaderCode);

    // input assembler
    vk::PipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo;
    inputAssemblyStateCreateInfo.sType = vk::StructureType::ePipelineInputAssemblyStateCreateInfo;
    inputAssemblyStateCreateInfo.setTopology(vk::PrimitiveTopology::eTriangleList);
    inputAssemblyStateCreateInfo.primitiveRestartEnable = vk::False;

    std::vector<vk::DynamicState> dynamicStages = {
            vk::DynamicState::eViewport,
            vk::DynamicState::eScissor
    };

    vk::PipelineDynamicStateCreateInfo dynamicStateCreateInfo;
    dynamicStateCreateInfo.sType = vk::StructureType::ePipelineDynamicStateCreateInfo;
    dynamicStateCreateInfo.dynamicStateCount = dynamicStages.size();
    dynamicStateCreateInfo.pDynamicStates = dynamicStages.data();

    vk::Viewport viewport;
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float) mSwapChainExtent.width;
    viewport.height = (float) mSwapChainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    vk::Rect2D scissor{};
    scissor.offset = vk::Offset2D{0, 0};
    scissor.extent = mSwapChainExtent;

    vk::PipelineViewportStateCreateInfo viewportStateCreateInfo;
    viewportStateCreateInfo.sType = vk::StructureType::ePipelineViewportStateCreateInfo;
    viewportStateCreateInfo.viewportCount = 1;
    viewportStateCreateInfo.setPViewports(&viewport);
    viewportStateCreateInfo.scissorCount = 1;
    viewportStateCreateInfo.setPScissors(&scissor);

    // vertex shader
    vk::PipelineVertexInputStateCreateInfo vertexInputStateCreateInfo;
    vertexInputStateCreateInfo.sType = vk::StructureType::ePipelineVertexInputStateCreateInfo;
    vertexInputStateCreateInfo.vertexBindingDescriptionCount = 0;
    vertexInputStateCreateInfo.pVertexBindingDescriptions = nullptr;
    vertexInputStateCreateInfo.vertexAttributeDescriptionCount = 0;
    vertexInputStateCreateInfo.pVertexAttributeDescriptions = nullptr;

    vk::PipelineShaderStageCreateInfo vertexShaderStageCreateInfo;
    vertexShaderStageCreateInfo.sType = vk::StructureType::ePipelineShaderStageCreateInfo;
    vertexShaderStageCreateInfo.setStage(vk::ShaderStageFlagBits::eVertex);
    vertexShaderStageCreateInfo.setModule(vertexModule);
    vertexShaderStageCreateInfo.setPName("main");
    vertexShaderStageCreateInfo.setPSpecializationInfo(nullptr);

    // tessellation

    // geometry shader

    // rasterization
    vk::PipelineRasterizationStateCreateInfo rasterizationStateCreateInfo;
    rasterizationStateCreateInfo.sType = vk::StructureType::ePipelineRasterizationStateCreateInfo;

    // 如果depthClampEnable设置为VK_TRUE，则超出近平面和远平面的片段将被夹紧，而不是丢弃它们。这在某些特殊情况下很有用，例如阴影贴图。使用此功能需要启用 GPU 功能。
    rasterizationStateCreateInfo.setDepthClampEnable(vk::False);

    // 如果rasterizerDiscardEnable设置为VK_TRUE，则几何图形永远不会通过光栅化阶段。这基本上禁用了帧缓冲区的任何输出。
    rasterizationStateCreateInfo.setRasterizerDiscardEnable(vk::False);

    //确定polygonMode如何为几何体生成片段。可以使用以下模式：
    //
    //VK_POLYGON_MODE_FILL：用碎片填充多边形区域
    //VK_POLYGON_MODE_LINE：多边形边缘绘制为线
    //VK_POLYGON_MODE_POINT：多边形顶点绘制为点
    rasterizationStateCreateInfo.setPolygonMode(vk::PolygonMode::eFill);
    // 使用填充以外的任何模式都需要设置 lineWidth :
    rasterizationStateCreateInfo.setLineWidth(1.0f);
    // 设置面剔除策略, 这里设置为反面被剔除
    rasterizationStateCreateInfo.setCullMode(vk::CullModeFlagBits::eBack);
    // 设置正面的方向, 这设置为顺时针为正面
    rasterizationStateCreateInfo.setFrontFace(vk::FrontFace::eClockwise);

    rasterizationStateCreateInfo.setDepthBiasEnable(vk::False);
    rasterizationStateCreateInfo.setDepthBiasConstantFactor(0.0f);
    rasterizationStateCreateInfo.setDepthBiasClamp(0.0f);
    rasterizationStateCreateInfo.setDepthBiasSlopeFactor(0.0f);

    // depth & stencil testing
    vk::PipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo;
    depthStencilStateCreateInfo.sType = vk::StructureType::ePipelineDepthStencilStateCreateInfo;



    // Multisampling
    vk::PipelineMultisampleStateCreateInfo multisampleStateCreateInfo;
    multisampleStateCreateInfo.sType = vk::StructureType::ePipelineMultisampleStateCreateInfo;
    multisampleStateCreateInfo.setSampleShadingEnable(vk::False);
    multisampleStateCreateInfo.setRasterizationSamples(vk::SampleCountFlagBits::e1);
    multisampleStateCreateInfo.setMinSampleShading(1.0f);
    multisampleStateCreateInfo.setPSampleMask(nullptr);
    multisampleStateCreateInfo.setAlphaToCoverageEnable(vk::False);
    multisampleStateCreateInfo.setAlphaToOneEnable(vk::False);

    // fragment shader
    vk::PipelineShaderStageCreateInfo fragmentShaderStageCreateInfo;
    fragmentShaderStageCreateInfo.sType = vk::StructureType::ePipelineShaderStageCreateInfo;
    fragmentShaderStageCreateInfo.setStage(vk::ShaderStageFlagBits::eFragment);
    fragmentShaderStageCreateInfo.setModule(fragmentModule);
    fragmentShaderStageCreateInfo.setPName("main");
    fragmentShaderStageCreateInfo.setPSpecializationInfo(nullptr);

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
    colorBlendAttachmentState.setBlendEnable(vk::True);

    colorBlendAttachmentState.setSrcColorBlendFactor(vk::BlendFactor::eSrcAlpha);
    colorBlendAttachmentState.setDstColorBlendFactor(vk::BlendFactor::eOneMinusSrcAlpha);
    colorBlendAttachmentState.setColorBlendOp(vk::BlendOp::eAdd);

    colorBlendAttachmentState.setSrcAlphaBlendFactor(vk::BlendFactor::eOne);
    colorBlendAttachmentState.setDstAlphaBlendFactor(vk::BlendFactor::eZero);
    colorBlendAttachmentState.setAlphaBlendOp(vk::BlendOp::eAdd);

    vk::PipelineColorBlendStateCreateInfo colorBlendStateCreateInfo;
    colorBlendStateCreateInfo.sType = vk::StructureType::ePipelineColorBlendStateCreateInfo;
    colorBlendStateCreateInfo.logicOpEnable = vk::False;
    colorBlendStateCreateInfo.logicOp = vk::LogicOp::eCopy;
    colorBlendStateCreateInfo.attachmentCount = 1;
    colorBlendStateCreateInfo.setPAttachments(&colorBlendAttachmentState);
    colorBlendStateCreateInfo.setBlendConstants(std::array<float, 4>{0.0f, 0.0f, 0.0f, 0.0f});

    vk::PipelineShaderStageCreateInfo shaderStageCreateInfos[] = {vertexShaderStageCreateInfo, fragmentShaderStageCreateInfo};

    vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo;
    pipelineLayoutCreateInfo.sType = vk::StructureType::ePipelineLayoutCreateInfo;
    pipelineLayoutCreateInfo.setSetLayoutCount(0);
    pipelineLayoutCreateInfo.setPSetLayouts(nullptr);
    pipelineLayoutCreateInfo.setPushConstantRangeCount(0);
    pipelineLayoutCreateInfo.setPPushConstantRanges(nullptr);

    mPipelineLayout = mDevice.createPipelineLayout(pipelineLayoutCreateInfo);

    vk::GraphicsPipelineCreateInfo graphicsPipelineCreateInfo;
    graphicsPipelineCreateInfo.sType = vk::StructureType::eGraphicsPipelineCreateInfo;
    graphicsPipelineCreateInfo.setStageCount(2);
    graphicsPipelineCreateInfo.setPStages(shaderStageCreateInfos);
    graphicsPipelineCreateInfo.setPVertexInputState(&vertexInputStateCreateInfo);
    graphicsPipelineCreateInfo.setPInputAssemblyState(&inputAssemblyStateCreateInfo);
    graphicsPipelineCreateInfo.setPViewportState(&viewportStateCreateInfo);
    graphicsPipelineCreateInfo.setPRasterizationState(&rasterizationStateCreateInfo);
    graphicsPipelineCreateInfo.setPMultisampleState(&multisampleStateCreateInfo);
    graphicsPipelineCreateInfo.setPDepthStencilState(&depthStencilStateCreateInfo);
    graphicsPipelineCreateInfo.setPColorBlendState(&colorBlendStateCreateInfo);
    graphicsPipelineCreateInfo.setPDynamicState(&dynamicStateCreateInfo);
    graphicsPipelineCreateInfo.setLayout(mPipelineLayout);
    graphicsPipelineCreateInfo.setRenderPass(mRenderPass);
    graphicsPipelineCreateInfo.setSubpass(0);
    graphicsPipelineCreateInfo.setBasePipelineHandle(VK_NULL_HANDLE);
    graphicsPipelineCreateInfo.setBasePipelineIndex(-1);

    auto pipelineCreateResult = mDevice.createGraphicsPipeline(VK_NULL_HANDLE, graphicsPipelineCreateInfo);
    if (pipelineCreateResult.result != vk::Result::eSuccess) {
        throw std::runtime_error("createGraphicsPipelines failed");
    }
    mGraphicsPipeline = pipelineCreateResult.value;

    mDevice.destroy(vertexModule);
    mDevice.destroy(fragmentModule);
}

vk::ShaderModule TriangleTest::createShaderModule(const std::vector<char> &code) {
    vk::ShaderModuleCreateInfo createInfo;
    createInfo.sType = vk::StructureType::eShaderModuleCreateInfo;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());

    return mDevice.createShaderModule(createInfo);
}

void TriangleTest::createFrameBuffers() {
    mSwapChainFrameBuffers.resize(mSwapChainImageViews.size());

    for (int i = 0; i < mSwapChainImageViews.size(); i++) {
        vk::ImageView attachments[] = {
                mSwapChainImageViews[i]
        };

        vk::FramebufferCreateInfo framebufferCreateInfo{};
        framebufferCreateInfo.sType = vk::StructureType::eFramebufferCreateInfo;
        framebufferCreateInfo.setRenderPass(mRenderPass);
        framebufferCreateInfo.setAttachmentCount(1);
        framebufferCreateInfo.setPAttachments(attachments);
        framebufferCreateInfo.setWidth(mSwapChainExtent.width);
        framebufferCreateInfo.setHeight(mSwapChainExtent.height);
        framebufferCreateInfo.setLayers(1);

        mSwapChainFrameBuffers[i] = mDevice.createFramebuffer(framebufferCreateInfo);
    }
}

void TriangleTest::createCommandPool() {
    QueueFamilyIndices queueFamilyIndices = findQueueFamilies(mPhysicalDevice);

    vk::CommandPoolCreateInfo commandPoolCreateInfo{};
    commandPoolCreateInfo.sType = vk::StructureType::eCommandPoolCreateInfo;
    commandPoolCreateInfo.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
    commandPoolCreateInfo.setQueueFamilyIndex(queueFamilyIndices.graphicQueueFamily.value());

    mCommandPool = mDevice.createCommandPool(commandPoolCreateInfo);
}

void TriangleTest::createCommandBuffers() {
    vk::CommandBufferAllocateInfo commandBufferAllocateInfo;
    commandBufferAllocateInfo.sType = vk::StructureType::eCommandBufferAllocateInfo;
    commandBufferAllocateInfo.setCommandPool(mCommandPool);
    commandBufferAllocateInfo.setLevel(vk::CommandBufferLevel::ePrimary);

    commandBufferAllocateInfo.setCommandBufferCount(MAX_FRAMES_IN_FLIGHT);


    // 返回 vector<CommandBuffer>, 取 [0]
    mCommandBuffers = mDevice.allocateCommandBuffers(commandBufferAllocateInfo);
    // mCommandBuffer 当 CommandPool 被销毁时会自动销毁, 不需要手动销毁
}

void TriangleTest::recordCommandBuffer(const vk::CommandBuffer &commandBuffer, uint32_t imageIndex) {
    vk::CommandBufferBeginInfo commandBufferBeginInfo;
    commandBufferBeginInfo.sType = vk::StructureType::eCommandBufferBeginInfo;
//    commandBufferBeginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
//    commandBufferBeginInfo.setPInheritanceInfo(nullptr);
    commandBuffer.begin(commandBufferBeginInfo);

    vk::RenderPassBeginInfo renderPassBeginInfo{};
    renderPassBeginInfo.sType = vk::StructureType::eRenderPassBeginInfo;
    renderPassBeginInfo.setRenderPass(mRenderPass);
    renderPassBeginInfo.setFramebuffer(mSwapChainFrameBuffers[imageIndex]);
    renderPassBeginInfo.renderArea.setOffset(vk::Offset2D{0, 0});
    renderPassBeginInfo.renderArea.setExtent(mSwapChainExtent);

    vk::ClearValue clearValue = vk::ClearValue{mClearColor};
    renderPassBeginInfo.setClearValueCount(1);
    renderPassBeginInfo.setPClearValues(&clearValue);

    commandBuffer.beginRenderPass(&renderPassBeginInfo, vk::SubpassContents::eInline);
    {
        commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, mGraphicsPipeline);

        vk::Viewport viewport;
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float) mSwapChainExtent.width;
        viewport.height = (float) mSwapChainExtent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        commandBuffer.setViewport(0, 1, &viewport);

        vk::Rect2D scissor{};
        scissor.offset = vk::Offset2D{0, 0};
        scissor.extent = mSwapChainExtent;
        commandBuffer.setScissor(0, 1, &scissor);

        // vertexCount：即使我们没有顶点缓冲区，从技术上讲我们仍然有 3 个顶点要绘制。
        // instanceCount：用于实例渲染，1如果您不这样做，请使用。
        // firstVertex：用作顶点缓冲区的偏移量，定义 的最小值gl_VertexIndex。
        // firstInstance：用作实例渲染的偏移量，定义 的最小值gl_InstanceIndex。
        commandBuffer.draw(3, 1, 0, 0);
    }
    commandBuffer.endRenderPass();
    commandBuffer.end();
}


void TriangleTest::drawFrame() {
    vk::Result result = mDevice.waitForFences(1, &mInFlightFences[mCurrentFrame], vk::True, std::numeric_limits<uint64_t>::max());
    if (result != vk::Result::eSuccess) {
        throw std::runtime_error("waitForFences failed");
    }
    result = mDevice.resetFences(1, &mInFlightFences[mCurrentFrame]);
    if (result != vk::Result::eSuccess) {
        throw std::runtime_error("resetFences failed");
    }

    auto acquireResult = mDevice.acquireNextImageKHR(mSwapChain, std::numeric_limits<uint64_t>::max(), mImageAvailableSemaphores[mCurrentFrame]);
    if (acquireResult.result != vk::Result::eSuccess) {
        throw std::runtime_error("acquireNextImageKHR failed");
    }
    uint32_t imageIndex = acquireResult.value;

    mCommandBuffers[mCurrentFrame].reset();
    recordCommandBuffer(mCommandBuffers[mCurrentFrame], imageIndex);

    vk::SubmitInfo submitInfo{};
    submitInfo.sType = vk::StructureType::eSubmitInfo;

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

    result = mGraphicQueue.submit(1, &submitInfo, mInFlightFences[mCurrentFrame]);
    if (result != vk::Result::eSuccess) {
        throw std::runtime_error("graphicQueue.submit failed");
    }

    vk::PresentInfoKHR presentInfo{};
    presentInfo.sType = vk::StructureType::ePresentInfoKHR;
    presentInfo.setWaitSemaphores(signalSemaphores);
    presentInfo.setSwapchains(mSwapChain);
    presentInfo.setImageIndices(imageIndex);

    result = mPresentQueue.presentKHR(presentInfo);
    if (result != vk::Result::eSuccess) {
        throw std::runtime_error("presentKHR failed");
    }

    mCurrentFrame = (mCurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void TriangleTest::cleanFrameBuffers() {
    for (auto swapChainFrameBuffer: mSwapChainFrameBuffers) {
        mDevice.destroy(swapChainFrameBuffer);
    }
}

void TriangleTest::createSyncObjects() {
    vk::SemaphoreCreateInfo semaphoreCreateInfo;
    semaphoreCreateInfo.sType = vk::StructureType::eSemaphoreCreateInfo;

    vk::FenceCreateInfo fenceCreateInfo;
    fenceCreateInfo.sType = vk::StructureType::eFenceCreateInfo;
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
    createInfo.messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eError
                                 | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning
                                 | vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo;

    createInfo.messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral
                             | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation
                             | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance
                             | vk::DebugUtilsMessageTypeFlagBitsEXT::eDeviceAddressBinding;
    createInfo.pfnUserCallback = debugCallback;
}