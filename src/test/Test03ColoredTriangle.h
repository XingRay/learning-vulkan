//
// Created by leixing on 2025/1/4.
//

#pragma once

#include "engine/Log.h"
#include "test/common/TestBase.h"
#include "test/common/glm.h"

#include "engine/VulkanEngine.h"
#include <GLFW/glfw3.h>

namespace test03 {
    struct Vertex {
        glm::vec3 position;
        glm::vec3 color;
    };

    class Test03ColoredTriangle : public test::TestBase {
    private:
        const int mFrameCount = 2;

        std::unique_ptr<engine::VulkanGraphicsEngine> mVulkanEngine;

    public:
        // 构造函数初始化基类 TestBase，并传递 name
        explicit Test03ColoredTriangle(GLFWwindow* window);

        // 初始化操作
        void init() override;

        // 检查是否准备好
        bool isReady() override;

        // 绘制三角形帧
        void drawFrame() override;

        // 清理操作
        void cleanup() override;
    };
} // test