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
#include "WindowsVulkanSurface.h"

namespace triangle {
    TriangleTest::TriangleTest() {

    }

    TriangleTest::~TriangleTest() {

    }

    void TriangleTest::main() {

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
        createLogicalDevice();
        createSwapChain();
        createRenderPass();
        createDescriptorSetLayout();
        createGraphicsPipeline();
        createCommandPool();
        createFrameBuffers();
        loadModel();
        createVertexBuffer();
        createIndexBuffer();
        createSyncObjects();
    }

    void TriangleTest::mainLoop() {
        while (!glfwWindowShouldClose(mWindow)) {
            glfwPollEvents();
            drawFrame();
        }

        // 等待所有任务执行完成
        mVulkanDevice->getDevice().waitIdle();
    }

    void TriangleTest::cleanUp() {
        vk::Device device = mVulkanDevice->getDevice();
        cleanUpSwapChain();

        mVulkanDescriptorSet.reset();

        device.destroy(mVertexBuffer);
        device.free(mVertexBufferMemory);

        device.destroy(mIndexBuffer);
        device.free(mIndexBufferMemory);

        mVulkanPipeline.reset();
        mVulkanRenderPass.reset();

        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            device.destroy(mImageAvailableSemaphores[i]);
            device.destroy(mRenderFinishedSemaphores[i]);
            device.destroy(mInFlightFences[i]);
        }

//        device.destroy(mCommandPool);
        mVulkanCommandPool.reset();

        mVulkanDevice.reset();

        if (mEnableValidationLayer) {
            destroyDebugUtilsMessengerExt(nullptr);
        }

        mVulkanSurface.reset();
        mVulkanInstance.reset();

        glfwDestroyWindow(mWindow);
        glfwTerminate();
    }

    void TriangleTest::createInstance() {
        std::cout << "createInstance\n";

        std::vector<const char *> extensions = getRequiredExtensions();
        std::cout << "instance extensions \n";
        for (const auto &extension: extensions) {
            std::cout << "    extension:" << extension << std::endl;
        }

        mVulkanInstance = std::make_unique<engine::VulkanInstance>(extensions, mValidationLayers);
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
        DestroyDebugUtilsMessengerEXT(mVulkanInstance->getInstance(), mDebugMessenger, pAllocator);
    }

    void TriangleTest::setupDebugMessenger() {
        std::cout << "setupDebugMessenger\n";
        if (!mEnableValidationLayer) {
            return;
        }

        vk::DebugUtilsMessengerCreateInfoEXT createInfo;
        populateDebugMessengerCreateInfo(createInfo);

        if (CreateDebugUtilsMessengerEXT(mVulkanInstance->getInstance(), &createInfo, nullptr, &mDebugMessenger) != vk::Result::eSuccess) {
            throw std::runtime_error("failed to set up debug messenger!");
        }
    }

    void TriangleTest::createLogicalDevice() {
        mVulkanDevice = std::make_unique<engine::VulkanDevice>(mVulkanInstance->getInstance(),
                                                               mVulkanSurface->getSurface(),
                                                               mValidationLayers,
                                                               mRequiredExtensions);
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

            if (device.getSurfaceSupportKHR(i, mVulkanSurface->getSurface())) {
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
        mVulkanSurface = std::make_unique<engine::WindowsVulkanSurface>(mVulkanInstance->getInstance(), mWindow);
    }

    void TriangleTest::createSwapChain() {
        vk::Extent2D currentExtent = mVulkanDevice->getCapabilities().currentExtent;
        LOG_D("currentExtent:[%d x %d]", currentExtent.width, currentExtent.height);

        mVulkanSwapchain = std::make_unique<engine::VulkanSwapchain>(*mVulkanDevice, *mVulkanSurface, currentExtent.width, currentExtent.height);
    }

    void TriangleTest::createRenderPass() {
        mVulkanRenderPass = std::make_unique<engine::VulkanRenderPass>(*mVulkanDevice, *mVulkanSwapchain);
    }

    void TriangleTest::createGraphicsPipeline() {
        vk::Device device = mVulkanDevice->getDevice();

        std::vector<char> vertexShaderCode = FileUtil::readFile("../output/shader/01_triangle/vertex.spv");
        std::vector<char> fragmentShaderCode = FileUtil::readFile("../output/shader/01_triangle/fragment.spv");

        std::vector<vk::VertexInputBindingDescription> bindingDescriptions;
        bindingDescriptions.push_back(app::Vertex::getBindingDescription());
        std::vector<vk::VertexInputAttributeDescription> attributeDescriptions;
        attributeDescriptions.push_back(app::Vertex::getAttributeDescription());


        vk::ShaderModule vertexModule = mVulkanDevice->createShaderModule(vertexShaderCode);
        vk::ShaderModule fragmentModule = mVulkanDevice->createShaderModule(fragmentShaderCode);

        mVulkanPipeline = std::make_unique<engine::VulkanPipeline>(*mVulkanDevice, *mVulkanSwapchain, *mVulkanDescriptorSet, *mVulkanRenderPass,
                                                                   vertexModule, fragmentModule,
                                                                   bindingDescriptions, attributeDescriptions);
        device.destroy(vertexModule);
        device.destroy(fragmentModule);
    }

    void TriangleTest::createFrameBuffers() {
        mVulkanFrameBuffer = std::make_unique<engine::VulkanFrameBuffer>(*mVulkanDevice, *mVulkanSwapchain, *mVulkanRenderPass, *mVulkanCommandPool);
    }

    void TriangleTest::createCommandPool() {
        mVulkanCommandPool = std::make_unique<engine::VulkanCommandPool>(*mVulkanDevice, MAX_FRAMES_IN_FLIGHT);
    }

    void TriangleTest::recordCommandBuffer(const vk::CommandBuffer &commandBuffer, uint32_t imageIndex) {
        vk::CommandBufferBeginInfo commandBufferBeginInfo;
//    commandBufferBeginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
//    commandBufferBeginInfo.setPInheritanceInfo(nullptr);
        commandBuffer.begin(commandBufferBeginInfo);

        vk::Rect2D renderArea;
        renderArea.setOffset(vk::Offset2D{0, 0})
                .setExtent(mVulkanSwapchain->getDisplaySize());

        vk::RenderPassBeginInfo renderPassBeginInfo{};
        renderPassBeginInfo.setRenderPass(mVulkanRenderPass->getRenderPass())
                .setFramebuffer(mVulkanFrameBuffer->getFrameBuffers()[imageIndex])
                .setRenderArea(renderArea);

        vk::ClearValue colorClearValue = vk::ClearValue{mClearColor};
        std::array<float, 4> depthStencil = {1.0f, 0, 0, 0};
        vk::ClearValue depthStencilClearValue = vk::ClearValue{vk::ClearColorValue(depthStencil)};
        std::array<vk::ClearValue, 2> clearValues = {colorClearValue, depthStencilClearValue};
        renderPassBeginInfo.setClearValueCount(clearValues.size())
                .setClearValues(clearValues);

        commandBuffer.beginRenderPass(&renderPassBeginInfo, vk::SubpassContents::eInline);

        vk::Viewport viewport;
        viewport.setX(0.0f)
                .setY(0.0f)
                .setWidth((float) mVulkanSwapchain->getDisplaySize().width)
                .setHeight((float) mVulkanSwapchain->getDisplaySize().height)
                .setMinDepth(0.0f)
                .setMaxDepth(1.0f);

        vk::Rect2D scissor{};
        scissor.setOffset(vk::Offset2D{0, 0})
                .setExtent(mVulkanSwapchain->getDisplaySize());

        vk::Buffer vertexBuffers[] = {mVertexBuffer};
        vk::DeviceSize offsets[] = {0};

        commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, mVulkanPipeline->getPipeline());
        commandBuffer.setViewport(0, 1, &viewport);
        commandBuffer.setScissor(0, 1, &scissor);
        commandBuffer.bindVertexBuffers(0, vertexBuffers, offsets);
        commandBuffer.bindIndexBuffer(mIndexBuffer, 0, vk::IndexType::eUint32);
        // vertexCount：即使我们没有顶点缓冲区，从技术上讲我们仍然有 3 个顶点要绘制。
        // instanceCount：用于实例渲染，1如果您不这样做，请使用。
        // firstVertex：用作顶点缓冲区的偏移量，定义 的最小值gl_VertexIndex。
        // firstInstance：用作实例渲染的偏移量，定义 的最小值gl_InstanceIndex。
//        commandBuffer.draw(mVertices.size(), 1, 0, 0);
        commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, mVulkanPipeline->getPipelineLayout(), 0, {mVulkanDescriptorSet->getDescriptorSets()[mCurrentFrame]}, nullptr);
        commandBuffer.drawIndexed(mIndices.size(), 1, 0, 0, 0);

        commandBuffer.endRenderPass();
        commandBuffer.end();
    }


    void TriangleTest::drawFrame() {
        vk::Result result = mVulkanDevice->getDevice().waitForFences(1, &mInFlightFences[mCurrentFrame], vk::True, std::numeric_limits<uint64_t>::max());
        if (result != vk::Result::eSuccess) {
            throw std::runtime_error("waitForFences failed");
        }

//    std::cout << "acquireNextImageKHR" << std::endl;
        auto [acquireResult, imageIndex] = mVulkanDevice->getDevice().acquireNextImageKHR(mVulkanSwapchain->getSwapChain(), std::numeric_limits<uint64_t>::max(),
                                                                                          mImageAvailableSemaphores[mCurrentFrame]);
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

        mVulkanCommandPool->getCommandBuffers()[mCurrentFrame].reset();
        recordCommandBuffer(mVulkanCommandPool->getCommandBuffers()[mCurrentFrame], imageIndex);

        // 仅在我们提交工作时重置栅栏
        result = mVulkanDevice->getDevice().resetFences(1, &mInFlightFences[mCurrentFrame]);
        if (result != vk::Result::eSuccess) {
            throw std::runtime_error("resetFences failed");
        }

        vk::SubmitInfo submitInfo{};

        vk::Semaphore waitSemaphores[] = {mImageAvailableSemaphores[mCurrentFrame]};
        vk::PipelineStageFlags waitStages[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
        submitInfo.setWaitSemaphoreCount(1);
        submitInfo.setPWaitSemaphores(waitSemaphores);
        submitInfo.setPWaitDstStageMask(waitStages);
        submitInfo.setCommandBufferCount(1);
        submitInfo.setPCommandBuffers(&mVulkanCommandPool->getCommandBuffers()[mCurrentFrame]);

        vk::Semaphore signalSemaphores[] = {mRenderFinishedSemaphores[mCurrentFrame]};
        submitInfo.setSignalSemaphoreCount(1);
        submitInfo.setPSignalSemaphores(signalSemaphores);

        result = mVulkanDevice->getGraphicsQueue().submit(1, &submitInfo, mInFlightFences[mCurrentFrame]);
        if (result != vk::Result::eSuccess) {
            throw std::runtime_error("graphicQueue.submit failed");
        }

        vk::PresentInfoKHR presentInfo{};
        presentInfo.setWaitSemaphores(signalSemaphores);
        presentInfo.setSwapchainCount(1);
        presentInfo.setPSwapchains(&mVulkanSwapchain->getSwapChain());
        presentInfo.setImageIndices(imageIndex);

        // https://github.com/KhronosGroup/Vulkan-Hpp/issues/599
        // 当出现图片不匹配时， cpp风格的 presentKHR 会抛出异常， 而不是返回 result， 而C风格的 presentKHR 接口会返回 result
        try {
            result = mVulkanDevice->getPresentQueue().presentKHR(presentInfo);
        } catch (const vk::OutOfDateKHRError &e) {
            std::cout << "mPresentQueue.presentKHR => OutOfDateKHRError" << std::endl;
            result = vk::Result::eErrorOutOfDateKHR;
        }

        if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR || mFrameBufferResized) {
            mFrameBufferResized = false;
            std::cout << "presentKHR: eErrorOutOfDateKHR or eSuboptimalKHR or mFrameBufferResized, recreateSwapChain" << std::endl;
            recreateSwapChain();
            return;
        } else if (result != vk::Result::eSuccess) {
            throw std::runtime_error("presentKHR failed");
        }

        mCurrentFrame = (mCurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    void TriangleTest::cleanFrameBuffers() {
        mVulkanFrameBuffer.reset();
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
            mImageAvailableSemaphores[i] = mVulkanDevice->getDevice().createSemaphore(semaphoreCreateInfo);
            mRenderFinishedSemaphores[i] = mVulkanDevice->getDevice().createSemaphore(semaphoreCreateInfo);
            mInFlightFences[i] = mVulkanDevice->getDevice().createFence(fenceCreateInfo);
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

        mVulkanDevice->getDevice().waitIdle();

        cleanUpSwapChain();

        createSwapChain();
        createColorResources();
        createDepthResources();
        createFrameBuffers();
    }

    void TriangleTest::cleanUpSwapChain() {
        mVulkanSwapchain.reset();

        cleanFrameBuffers();
        cleanColorResources();
        cleanDepthResources();
    }

    void TriangleTest::createVertexBuffer() {
        vk::DeviceSize bufferSize = sizeof(mVertices[0]) * mVertices.size();

        mVulkanVertexBuffer = std::make_unique<engine::VulkanVertexBuffer>(*mVulkanDevice, bufferSize);
        mVertexBuffer = mVulkanVertexBuffer->getVertexBuffer();
        mVertexBufferMemory = mVulkanVertexBuffer->getVertexBufferMemory();


//        auto [stagingBuffer, stagingBufferMemory] = createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc,
//                                                                 vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
//        void *data = mVulkanDevice->getDevice().mapMemory(stagingBufferMemory, 0, bufferSize, static_cast<vk::MemoryMapFlags>(0)/*vk::MemoryMapFlagBits::ePlacedEXT*/);
//        {
//            memcpy(data, mVertices.data(), (size_t) bufferSize);
//        }
//        mVulkanDevice->getDevice().unmapMemory(stagingBufferMemory);
//
//        copyBuffer(stagingBuffer, mVertexBuffer, bufferSize);
//
//        mVulkanDevice->getDevice().destroy(stagingBuffer);
//        mVulkanDevice->getDevice().freeMemory(stagingBufferMemory);


        mVulkanVertexBuffer->updateByStageBuffer(*mVulkanCommandPool, mVertices.data(), bufferSize);


    }

    void TriangleTest::createIndexBuffer() {
        vk::DeviceSize bufferSize = sizeof(mIndices[0]) * mIndices.size();

        auto [stagingBuffer, stagingBufferMemory] = createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc,
                                                                 vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
        void *data = mVulkanDevice->getDevice().mapMemory(stagingBufferMemory, 0, bufferSize, static_cast<vk::MemoryMapFlags>(0)/*vk::MemoryMapFlagBits::ePlacedEXT*/);
        {
            memcpy(data, mIndices.data(), (size_t) bufferSize);
        }
        mVulkanDevice->getDevice().unmapMemory(stagingBufferMemory);

        std::tie(mIndexBuffer, mIndexBufferMemory) = createBuffer(bufferSize,
                                                                  vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer,
                                                                  vk::MemoryPropertyFlagBits::eDeviceLocal);

        copyBuffer(stagingBuffer, mIndexBuffer, bufferSize);

        mVulkanDevice->getDevice().destroy(stagingBuffer);
        mVulkanDevice->getDevice().freeMemory(stagingBufferMemory);
    }

    uint32_t TriangleTest::findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties) {
        vk::PhysicalDeviceMemoryProperties memoryProperties = mVulkanDevice->getPhysicalDevice().getMemoryProperties();

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

        vk::Buffer buffer = mVulkanDevice->getDevice().createBuffer(bufferCreateInfo);
        vk::MemoryRequirements memoryRequirements = mVulkanDevice->getDevice().getBufferMemoryRequirements(buffer);

        uint32_t memoryType = findMemoryType(memoryRequirements.memoryTypeBits, properties);
        vk::MemoryAllocateInfo memoryAllocateInfo{};
        memoryAllocateInfo.setAllocationSize(memoryRequirements.size)
                .setMemoryTypeIndex(memoryType);

        vk::DeviceMemory bufferMemory = mVulkanDevice->getDevice().allocateMemory(memoryAllocateInfo);
        mVulkanDevice->getDevice().bindBufferMemory(buffer, bufferMemory, 0);

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
        mVulkanDescriptorSet = std::make_unique<engine::VulkanDescriptorSet>(*mVulkanDevice, MAX_FRAMES_IN_FLIGHT);
    }

    std::pair<vk::Image, vk::DeviceMemory>
    TriangleTest::createImage(uint32_t width, uint32_t height, uint32_t mipLevels, vk::SampleCountFlagBits numSamples,
                              vk::Format format, vk::ImageTiling imageTiling, vk::ImageUsageFlags imageUsage, vk::MemoryPropertyFlags memoryProperty) {

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

        vk::Image image = mVulkanDevice->getDevice().createImage(imageCreateInfo);

        vk::MemoryRequirements memoryRequirements = mVulkanDevice->getDevice().getImageMemoryRequirements(image);

        vk::MemoryAllocateInfo memoryAllocateInfo;
        uint32_t memoryType = findMemoryType(memoryRequirements.memoryTypeBits, memoryProperty);
        memoryAllocateInfo.setAllocationSize(memoryRequirements.size)
                .setMemoryTypeIndex(memoryType);

        vk::DeviceMemory imageMemory = mVulkanDevice->getDevice().allocateMemory(memoryAllocateInfo);

        mVulkanDevice->getDevice().bindImageMemory(image, imageMemory, 0);

        return {image, imageMemory};
    }

    vk::CommandBuffer TriangleTest::beginSingleTimeCommands() {
        vk::CommandBufferAllocateInfo commandBufferAllocateInfo{};
        commandBufferAllocateInfo.setLevel(vk::CommandBufferLevel::ePrimary)
                .setCommandPool(mVulkanCommandPool->getCommandPool())
                .setCommandBufferCount(1);

        vk::CommandBuffer commandBuffer = mVulkanDevice->getDevice().allocateCommandBuffers(commandBufferAllocateInfo)[0];

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

        mVulkanDevice->getGraphicsQueue().submit(submitInfo);
        mVulkanDevice->getGraphicsQueue().waitIdle();

        mVulkanDevice->getDevice().freeCommandBuffers(mVulkanCommandPool->getCommandPool(), commandBuffer);
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

    vk::ImageView TriangleTest::createImageView(const vk::Image &image, vk::Format format, vk::ImageAspectFlags imageAspect, uint32_t mipLevels) {
        vk::ImageViewCreateInfo imageViewCreateInfo;

        imageViewCreateInfo.setImage(image)
                .setViewType(vk::ImageViewType::e2D)
                .setFormat(format);

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

        return mVulkanDevice->getDevice().createImageView(imageViewCreateInfo);
    }

    void TriangleTest::createDepthResources() {
        std::cout << "createDepthResources" << std::endl;
        std::vector<vk::Format> candidate;

        vk::Format depthFormat = findDepthFormat();

        std::tie(mDepthImage, mDepthDeviceMemory) = createImage(mVulkanSwapchain->getDisplaySize().width, mVulkanSwapchain->getDisplaySize().height, 1, mVulkanDevice->getMsaaSamples(),
                                                                depthFormat,
                                                                vk::ImageTiling::eOptimal,
                                                                vk::ImageUsageFlagBits::eDepthStencilAttachment,
                                                                vk::MemoryPropertyFlagBits::eDeviceLocal);
        mDepthImageView = createImageView(mDepthImage, depthFormat, vk::ImageAspectFlagBits::eDepth, 1);
        transitionImageLayout(mDepthImage, depthFormat, vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthStencilAttachmentOptimal, 1);
    }

    vk::Format TriangleTest::findSupportedFormat(const std::vector<vk::Format> &candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features) {
        for (const auto &format: candidates) {
            vk::FormatProperties properties = mVulkanDevice->getPhysicalDevice().getFormatProperties(format);
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
        mVulkanDevice->getDevice().destroy(mDepthImage);
        mVulkanDevice->getDevice().destroy(mDepthImageView);
        mVulkanDevice->getDevice().freeMemory(mDepthDeviceMemory);
    }

    void TriangleTest::loadModel() {
        mVertices = {
                {{0.0f,  -0.5f, 0.0f}},  // Bottom vertex
                {{0.5f,  0.5f,  0.0f}},  // Right vertex
                {{-0.5f, 0.5f,  0.0f}}   // Left vertex
        };

        mIndices = {0, 2, 1, 0, 1, 2};
    }

    void TriangleTest::createColorResources() {
        vk::Format colorFormat = mVulkanSwapchain->getSurfaceFormat().format;

        std::tie(mColorImage, mColorDeviceMemory) = createImage(mVulkanSwapchain->getDisplaySize().width, mVulkanSwapchain->getDisplaySize().height, 1, mVulkanDevice->getMsaaSamples(), colorFormat,
                                                                vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eTransientAttachment | vk::ImageUsageFlagBits::eColorAttachment,
                                                                vk::MemoryPropertyFlagBits::eDeviceLocal);
        mColorImageView = createImageView(mColorImage, colorFormat, vk::ImageAspectFlagBits::eColor, 1);
    }

    void TriangleTest::cleanColorResources() {
        mVulkanDevice->getDevice().destroy(mColorImage);
        mVulkanDevice->getDevice().destroy(mColorImageView);
        mVulkanDevice->getDevice().freeMemory(mColorDeviceMemory);
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
}
