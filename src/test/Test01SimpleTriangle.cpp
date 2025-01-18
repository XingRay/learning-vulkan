//
// Created by leixing on 2025/1/4.
//

#include "Test01SimpleTriangle.h"

#include "engine/vulkan_wrapper/winows/WindowsVulkanSurface.h"
#include "FileUtil.h"

namespace test01 {
    Test01SimpleTriangle::Test01SimpleTriangle(GLFWwindow *window)
        : TestBase() {
        LOG_D("Test01SimpleTriangle::Test01SimpleTriangle");

        std::vector<std::string> instanceExtensions = {
            VK_KHR_SURFACE_EXTENSION_NAME,

            // old version
            VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
            // new version
            VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
        };

        std::vector<std::string> layers = {
            "VK_LAYER_KHRONOS_validation"
        };

        std::vector<std::string> deviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };

        uint32_t glfwRequiredExtensionCount = 0;
        // 查询 glfw 需要的 vulkan 扩展
        const char **glfwRequiredExtensions = glfwGetRequiredInstanceExtensions(&glfwRequiredExtensionCount);
        LOG_D("glfwRequiredExtensions:");
        for (int i = 0; i < glfwRequiredExtensionCount; i++) {
            LOG_D("  %s", glfwRequiredExtensions[i]);
            instanceExtensions.push_back(glfwRequiredExtensions[i]);
        }

        std::vector<char> vertexShaderCode = FileUtil::readFile("../output/shaders/01_triangle.vert.spv");
        std::vector<char> fragmentShaderCode = FileUtil::readFile("../output/shaders/01_triangle.frag.spv");

        std::unique_ptr<engine::VulkanGraphicsEngine> engine = engine::VulkanEngineBuilder{}
                .layers({}, layers)
                .extensions({}, instanceExtensions)
                .asGraphics()
                .deviceExtensions(std::move(deviceExtensions))
                .surface(engine::WindowsVulkanSurface::surfaceBuilder(window))
                .enableMsaa()
                .physicalDeviceAsDefault()
                .shader([&](engine::VulkanShaderConfigure &shaderConfigure) {
                    shaderConfigure
                            .vertexShaderCode(std::move(vertexShaderCode))
                            .fragmentShaderCode(std::move(fragmentShaderCode))
                            .vertex([](engine::VulkanVertexConfigure &vertexConfigure) {
                                vertexConfigure
                                        .binding(0)
                                        .size(sizeof(Vertex))
                                        .addAttribute(ShaderFormat::Vec3);
                            });
                })
                .build();

        mVulkanEngine = std::move(engine);
    }

    void Test01SimpleTriangle::init() {
        std::vector<Vertex> vertices = {
            {{1.0f, -1.0f, 0.0f}},
            {{-1.0f, -1.0f, 0.0f}},
            {{0.0f, 1.0f, 0.0f}},
        };

        std::vector<uint32_t> indices = {0, 1, 2};

        mVulkanEngine->createStagingTransferVertexBuffer(vertices.size() * sizeof(Vertex));
        mVulkanEngine->updateVertexBuffer(vertices);

        mVulkanEngine->createStagingTransferIndexBuffer(indices.size() * sizeof(uint32_t));
        mVulkanEngine->updateIndexBuffer(indices);
    }

    // 检查是否准备好
    bool Test01SimpleTriangle::isReady() {
        return true;
    }

    // 绘制三角形帧
    void Test01SimpleTriangle::drawFrame() {
        mVulkanEngine->drawFrame();
    }

    // 清理操作
    void Test01SimpleTriangle::cleanup() {
        mVulkanEngine.reset();
    }
} // test
