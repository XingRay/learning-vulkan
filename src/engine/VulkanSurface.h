//
// Created by leixing on 2024/12/22.
//

#ifndef VULKANDEMO_VULKANSURFACE_H
#define VULKANDEMO_VULKANSURFACE_H

#include "vulkan/vulkan.hpp"

namespace engine {

    class VulkanSurface {
    public:
        virtual ~VulkanSurface() = default;

        // 禁用拷贝和赋值操作符
        VulkanSurface(const VulkanSurface &) = delete;
        VulkanSurface &operator=(const VulkanSurface &) = delete;

        // 获取 Vulkan Surface
        [[nodiscard]]
        virtual vk::SurfaceKHR getSurface() const = 0;

    protected:
        VulkanSurface() = default;
    };

} // namespace engine


#endif //VULKANDEMO_VULKANSURFACE_H
