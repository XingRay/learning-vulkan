//
// Created by leixing on 2024/5/12.
//

#include "EngineTest.h"

#include <iostream>
#include "FileUtil.h"
#include "WindowsVulkanSurface.h"
#include "Log.h"

//// https://github.com/nothings/stb/issues/917
//// #define STB_IMAGE_IMPLEMENTATION 不能放在头文件中， 必须放在源文件中
//#define STB_IMAGE_IMPLEMENTATION
//
//#include <stb_image.h>
//
////#define STB_IMAGE_WRITE_IMPLEMENTATION
////#include "stb_image_write.h"
//
////#define STB_IMAGE_RESIZE_IMPLEMENTATION
////#include "stb_image_resize.h"


//#define TINYOBJLOADER_IMPLEMENTATION
//
//#include <tiny_obj_loader.h>



namespace engine {


    EngineTest::EngineTest() {

    }

    EngineTest::~EngineTest() {

    }

    void EngineTest::main() {
        initWindow();
        initVulkan();
        mainLoop();
        cleanUp();
    }

    void EngineTest::initWindow() {
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
            auto app = reinterpret_cast<EngineTest *>(glfwGetWindowUserPointer(window));
            std::cout << "app->mFrameBufferResized, width:" << width << " height:" << height << std::endl;
            app->mEngine->mFrameBufferResized = true;
        });
    }

    void EngineTest::initVulkan() {
        std::vector<const char *> instanceExtensions = getRequiredInstanceExtensions();
        if (mEnableValidationLayer) {
            instanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
            instanceExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
        }

        std::vector<const char *> layers = {
                "VK_LAYER_KHRONOS_validation"
        };

        mEngine = std::make_unique<VulkanEngine>(instanceExtensions, layers, MAX_FRAMES_IN_FLIGHT);

        std::unique_ptr<VulkanSurface> surface = std::make_unique<WindowsVulkanSurface>(mEngine->getVKInstance(), mWindow);

        std::vector<const char *> deviceExtensions;
        deviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

        LOG_D("Required device extensions:");
        for (const auto &extension: deviceExtensions) {
            LOG_D("    extension:%s", extension);
        }

        std::vector<char> vertexShaderCode = FileUtil::readFile("../output/shader/01_triangle/vertex.spv");
        std::vector<char> fragmentShaderCode = FileUtil::readFile("../output/shader/01_triangle/fragment.spv");
        mEngine->initVulkan(surface, deviceExtensions, vertexShaderCode, fragmentShaderCode);
    }

    void EngineTest::mainLoop() {
        while (!glfwWindowShouldClose(mWindow)) {
            glfwPollEvents();
            if (mEngine->isVulkanReady()) {
                mEngine->drawFrame();
            }
        }

        // 等待所有任务执行完成
        mEngine->getVKDevice().waitIdle();
    }

    void EngineTest::cleanUp() {
        glfwDestroyWindow(mWindow);
        glfwTerminate();
    }

    std::vector<const char *> EngineTest::getRequiredInstanceExtensions() {
        uint32_t glfwRequiredExtensionCount = 0;
        // 查询 glfw 需要的 vulkan 扩展
        const char **glfwRequiredExtensions = glfwGetRequiredInstanceExtensions(&glfwRequiredExtensionCount);
        LOG_D("glfwRequiredExtensions:");
        for (int i = 0; i < glfwRequiredExtensionCount; i++) {
            LOG_D("    %s", glfwRequiredExtensions[i]);
        }
        LOG_D("glfwRequiredExtensions: end");

        std::vector<const char *> extensions(glfwRequiredExtensions, glfwRequiredExtensions + glfwRequiredExtensionCount);
        return extensions;
    }


}


