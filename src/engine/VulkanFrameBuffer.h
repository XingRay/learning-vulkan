//
// Created by leixing on 2024/12/31.
//

#ifndef VULKANDEMO_VULKANFRAMEBUFFER_H
#define VULKANDEMO_VULKANFRAMEBUFFER_H

#include "vulkan/vulkan.hpp"
#include "VulkanDevice.h"
#include "VulkanSwapchain.h"
#include "VulkanRenderPass.h"
#include "VulkanCommandPool.h"

namespace engine {

    class VulkanFrameBuffer {
    private:
        const VulkanDevice &mDevice;

        vk::Image mColorImage;
        vk::DeviceMemory mColorDeviceMemory;
        vk::ImageView mColorImageView;

        vk::Image mDepthImage;
        vk::DeviceMemory mDepthDeviceMemory;
        vk::ImageView mDepthImageView;

        std::vector<vk::Framebuffer> mFrameBuffers;

    public:
        VulkanFrameBuffer(const VulkanDevice &device, const VulkanSwapchain &vulkanSwapchain, const VulkanRenderPass &vulkanRenderPass, const VulkanCommandPool &commandPool);

        ~VulkanFrameBuffer();

        [[nodiscard]]
        const std::vector<vk::Framebuffer> &getFrameBuffers() const;

    private:

        vk::Format findDepthFormat(const vk::PhysicalDevice &physicalDevice);

        vk::Format findSupportedFormat(const vk::PhysicalDevice &physicalDevice, const std::vector<vk::Format> &candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features);

        void recordTransitionImageLayoutCommand(const vk::CommandBuffer &commandBuffer,
                                                vk::Image image,
                                                vk::Format format,
                                                vk::ImageLayout oldImageLayout,
                                                vk::ImageLayout newImageLayout,
                                                uint32_t mipLevels);

        bool hasStencilComponent(vk::Format format);
    };

} // engine

#endif //VULKANDEMO_VULKANFRAMEBUFFER_H
