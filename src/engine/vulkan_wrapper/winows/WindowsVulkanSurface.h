//
// Created by leixing on 2025/1/2.
//

#pragma once
#ifdef WIN32

#include <functional>

#include <vulkan/vulkan.hpp>

#include "engine/vulkan_wrapper/VulkanSurface.h"
#include "engine/vulkan_wrapper/VulkanInstance.h"

// That way GLFW will include its own definitions and automatically load the Vulkan header with it.
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace engine{
    class WindowsVulkanSurface : public VulkanSurface {

    private:
        vk::Instance mInstance;
        vk::SurfaceKHR mSurface;

    public:
        WindowsVulkanSurface(const vk::Instance& instance, GLFWwindow *window);

        ~WindowsVulkanSurface() override;

        [[nodiscard]]
        vk::SurfaceKHR getSurface() const override { return mSurface; }

        static std::function<std::unique_ptr<VulkanSurface>(const VulkanInstance &)> surfaceBuilder(GLFWwindow *window);
    };
}


#endif // WIN32
