//
// Created by leixing on 2025/1/2.
//

#include "WindowsVulkanSurface.h"
#include "Log.h"

namespace engine {
    WindowsVulkanSurface::WindowsVulkanSurface(const vk::Instance &instance, GLFWwindow *window) : mInstance(instance) {
        LOG_D("WindowsVulkanSurface::WindowsVulkanSurface");
        VkResult result = glfwCreateWindowSurface((VkInstance) instance, window, nullptr, (VkSurfaceKHR *) &mSurface);
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
}