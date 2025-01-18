//
// Created by leixing on 2025/1/4.
//

#include "Test04MvpMatrix.h"

#include <chrono>
#include <vulkan_wrapper/winows/WindowsVulkanSurface.h>

#include "FileUtil.h"

namespace test04 {
    Test04MvpMatrix::Test04MvpMatrix(GLFWwindow *window): TestBase(), mMvpMatrix(glm::mat4(1.0f)), mWindow(window) {
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

        std::vector<char> vertexShaderCode = FileUtil::readFile("../output/shaders/04_mvp_matrix.vert.spv");
        std::vector<char> fragmentShaderCode = FileUtil::readFile("../output/shaders/04_mvp_matrix.frag.spv");

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
                            })
                            .addPushConstant(sizeof(glm::mat4), 0, vk::ShaderStageFlagBits::eVertex);
                })
                .build();

        mVulkanEngine = std::move(engine);
    }

    void Test04MvpMatrix::init() {
        // x轴朝右, y轴朝下, z轴朝前, 右手系 (x,y)->z
        std::vector<Vertex> vertices = {
            {{1.0f, -1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
            {{-1.0f, -1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
            {{0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
        };

        std::vector<uint32_t> indices = {0, 1, 2};

        mMvpMatrix = MvpMatrix{};
        float scale = 1.0f;

        glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(scale, scale, scale));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        mMvpMatrix.model = model;
        mMvpMatrix.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f),
                                      glm::vec3(0.0f, 0.0f, 0.0f),
                                      glm::vec3(1.0f, 1.0f, 0.0f));

        int windowWidth, windowHeight;
        glfwGetWindowSize(mWindow, &windowWidth, &windowHeight);
        mMvpMatrix.proj = glm::perspective(glm::radians(45.0f), (float) windowWidth / (float) windowHeight, 0.1f, 10.0f);

        mVulkanEngine->createStagingTransferVertexBuffer(vertices.size() * sizeof(Vertex));
        mVulkanEngine->updateVertexBuffer(vertices);

        mVulkanEngine->createStagingTransferIndexBuffer(indices.size() * sizeof(uint32_t));
        mVulkanEngine->updateIndexBuffer(indices);
    }

    // 检查是否准备好
    bool Test04MvpMatrix::isReady() {
        return true;
    }

    // 绘制三角形帧
    void Test04MvpMatrix::drawFrame() {
        static auto startTime = std::chrono::high_resolution_clock::now();
        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

        float scale = 1.0f;

        glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(scale, scale, scale));
        model = glm::rotate(model, time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        mMvpMatrix.model = model;

        //        mMvpMatrix.model = glm::mat4(1.0f); // 单位矩阵
        //        mMvpMatrix.view = glm::mat4(1.0f);  // 单位矩阵
        //        mMvpMatrix.proj = glm::mat4(1.0f);  // 单位矩阵
        glm::mat4 mvp = mMvpMatrix.proj * mMvpMatrix.view * mMvpMatrix.model;
        mVulkanEngine->updatePushConstant(0, &(mvp));

        mVulkanEngine->drawFrame();
    }

    // 清理操作
    void Test04MvpMatrix::cleanup() {
        mVulkanEngine.reset();
    }
} // test
