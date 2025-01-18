//
// Created by leixing on 2024/12/26.
//

#pragma once

#include "vulkan/vulkan.hpp"

#include "engine/vulkan_wrapper/VulkanSwapchain.h"
#include "engine/Log.h"

namespace engine {
    class VulkanRenderPass {
    private://fields
        const VulkanDevice& mDevice;
        vk::RenderPass mRenderPass;

    public://methods
        VulkanRenderPass(const VulkanDevice &vulkanDevice, const VulkanSwapchain &vulkanSwapchain);

        ~VulkanRenderPass();

        [[nodiscard]]
        const vk::RenderPass &getRenderPass() const;

        static vk::Format findDepthFormat(const vk::PhysicalDevice &physicalDevice);

        static vk::Format findSupportedFormat(const vk::PhysicalDevice &physicalDevice,
                                              const std::vector<vk::Format> &candidates,
                                              vk::ImageTiling tiling,
                                              vk::FormatFeatureFlags features);
    };
}
