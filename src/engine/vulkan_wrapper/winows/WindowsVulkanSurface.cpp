//
// Created by leixing on 2025/1/2.
//
#ifdef WIN32

#include "WindowsVulkanSurface.h"
#include "engine/Log.h"

namespace engine {
    WindowsVulkanSurface::WindowsVulkanSurface(const vk::Instance &instance, GLFWwindow *window) : mInstance(instance) {
        LOG_D("WindowsVulkanSurface::WindowsVulkanSurface");
        VkResult result = glfwCreateWindowSurface(instance, window, nullptr, reinterpret_cast<VkSurfaceKHR *>(&mSurface));
        if (result != VK_SUCCESS) {
            LOG_E("failed to create surface on windows !");
            throw std::runtime_error("failed to create surface on windows !");
        }
    }

    WindowsVulkanSurface::~WindowsVulkanSurface() {
        LOG_D("WindowsVulkanSurface::~WindowsVulkanSurface()");
        if (mSurface != nullptr) {
            mInstance.destroy(mSurface);
        } else {
            LOG_W("surface is null");
        }
    }

    std::function<std::unique_ptr<VulkanSurface>(const VulkanInstance &)> WindowsVulkanSurface::surfaceBuilder(GLFWwindow *window) {
        return [window](const VulkanInstance &instance) -> std::unique_ptr<VulkanSurface> {
            // 创建 WindowsVulkanSurface 实例
            return std::make_unique<WindowsVulkanSurface>(instance.getInstance(), window);
        };
    }
}

#endif // WIN32
