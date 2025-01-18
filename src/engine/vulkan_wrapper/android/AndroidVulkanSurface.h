//
// Created by leixing on 2024/12/22.
//
#ifdef ANDROID

#pragma once

#include <vulkan/vulkan.hpp>

#include "engine/vulkan_wrapper/VulkanSurface.h"
#include "engine/vulkan_wrapper/VulkanInstance.h"

namespace engine {

    class AndroidVulkanSurface : public VulkanSurface {

    private:
        vk::Instance mInstance;
        vk::SurfaceKHR mSurface;

    public:
        AndroidVulkanSurface(const vk::Instance &instance, ANativeWindow *window);

        ~AndroidVulkanSurface() override;

        [[nodiscard]]
        vk::SurfaceKHR getSurface() const override { return mSurface; }

        static std::function<std::unique_ptr<VulkanSurface>(const VulkanInstance &)> surfaceBuilder(ANativeWindow *window);
    };

} // engine

#endif