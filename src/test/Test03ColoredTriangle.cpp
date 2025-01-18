//
// Created by leixing on 2025/1/4.
//

#include "Test03ColoredTriangle.h"

#include <vulkan_wrapper/winows/WindowsVulkanSurface.h>

#include "FileUtil.h"
#include "engine/VulkanUtil.h"

namespace test03 {
    Test03ColoredTriangle::Test03ColoredTriangle(GLFWwindow *window): TestBase() {
        LOG_D("Test03ColoredTriangle::Test03ColoredTriangle");

        std::vector<std::string> instanceExtensions = {
            VK_KHR_SURFACE_EXTENSION_NAME,
            "VK_KHR_android_surface",

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

        std::vector<char> vertexShaderCode = FileUtil::readFile( "../output/shaders/03_colored_triangle.vert.spv");
        std::vector<char> fragmentShaderCode = FileUtil::readFile("../output/shaders/03_colored_triangle.frag.spv");

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
                            .fragmentShaderCode(std::move(std::move(fragmentShaderCode)))
                            .vertex([](engine::VulkanVertexConfigure &vertexConfigure) {
                                vertexConfigure
                                        .binding(0)
                                        .size(sizeof(Vertex))
                                        .addAttribute(ShaderFormat::Vec3)
                                        //.addAttribute(1, 0, ShaderFormat::Vec3, sizeof(ShaderFormat::Vec3)) //error
                                        //.addAttribute(1, 0, ShaderFormat::Vec3, engine::VulkanUtil::getFormatSize(vk::Format::eR32G32B32Sfloat)); //ok
                                        .addAttribute(ShaderFormat::Vec3); //simple, binding set as vertexConfigure#binding, location and offset auto calc
                            });
                })
                .build();

        mVulkanEngine = std::move(engine);
    }

    void Test03ColoredTriangle::init() {
        std::vector<Vertex> vertices = {
            {{1.0f, -1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
            {{-1.0f, -1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
            {{0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
        };

        std::vector<uint32_t> indices = {0, 1, 2};

        mVulkanEngine->createStagingTransferVertexBuffer(vertices.size() * sizeof(Vertex));
        mVulkanEngine->updateVertexBuffer(vertices);

        mVulkanEngine->createStagingTransferIndexBuffer(indices.size() * sizeof(uint32_t));
        mVulkanEngine->updateIndexBuffer(indices);
    }

    // 检查是否准备好
    bool Test03ColoredTriangle::isReady() {
        return true;
    }

    // 绘制三角形帧
    void Test03ColoredTriangle::drawFrame() {
        mVulkanEngine->drawFrame();
    }

    // 清理操作
    void Test03ColoredTriangle::cleanup() {
        mVulkanEngine.reset();
    }
} // test