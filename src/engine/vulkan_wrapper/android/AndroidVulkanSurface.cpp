//
// Created by leixing on 2024/12/22.
//
#ifdef ANDROID

#include "AndroidVulkanSurface.h"
#include "engine/Log.h"

namespace engine {
    AndroidVulkanSurface::AndroidVulkanSurface(const vk::Instance &instance, ANativeWindow *window) : mInstance(instance) {
        LOG_D("AndroidSurface::AndroidSurface");
        vk::AndroidSurfaceCreateInfoKHR createInfo = vk::AndroidSurfaceCreateInfoKHR{}
                .setFlags(vk::AndroidSurfaceCreateFlagsKHR{})
                .setWindow(window);

        try {
            mSurface = instance.createAndroidSurfaceKHR(createInfo);
        } catch (vk::SystemError &err) {
            LOG_E("Failed to create Android surface: %s", std::string(err.what()).data());
            throw std::runtime_error("Failed to create Android surface: " + std::string(err.what()));
        }
    }

    AndroidVulkanSurface::~AndroidVulkanSurface() {
        LOG_D("AndroidSurface::~AndroidSurface()");
        if (mSurface != nullptr) {
            mInstance.destroy(mSurface);
        } else {
            LOG_W("surface is null");
        }
    }

    std::function<std::unique_ptr<VulkanSurface>(const VulkanInstance &)> AndroidVulkanSurface::surfaceBuilder(ANativeWindow *window) {
        return [window](const VulkanInstance &instance) -> std::unique_ptr<VulkanSurface> {
            // 创建 AndroidVulkanSurface 实例
            return std::make_unique<AndroidVulkanSurface>(instance.getInstance(), window);
        };
    }

} // engine

#endif