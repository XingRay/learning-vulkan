//
// Created by leixing on 2024/12/30.
//

#pragma once

#include "vulkan/vulkan.hpp"
#include "engine/vulkan_wrapper/VulkanDevice.h"
#include "engine/vulkan_wrapper/VulkanSwapchain.h"
#include "engine/vulkan_wrapper/VulkanRenderPass.h"
#include "engine/vulkan_wrapper/VulkanPipeline.h"

#include <functional>

namespace engine {

    class VulkanCommandPool {
    private:
        const VulkanDevice &mDevice;

        vk::CommandPool mCommandPool;

        std::vector<vk::CommandBuffer> mCommandBuffers;

    public:
        VulkanCommandPool(const VulkanDevice &device, uint32_t commandBufferCount);

        ~VulkanCommandPool();

        const vk::CommandPool& getCommandPool() const;

        const std::vector<vk::CommandBuffer>& getCommandBuffers() const;

        vk::CommandBuffer allocateCommand() const;

        void submitOneTimeCommand(const std::function<void(const vk::CommandBuffer&)> &command) const;

        void submitCommand(const vk::CommandBuffer &commandBuffer) const;

        static void recordCommand(const vk::CommandBuffer &commandBuffer, vk::CommandBufferUsageFlagBits usage, const std::function<void(const vk::CommandBuffer &)> &command);

        void recordCommandInRenderPass(const vk::CommandBuffer &commandBuffer,
                                       const vk::Framebuffer &frameBuffer,
                                       const vk::RenderPass &renderPass,
                                       const vk::Pipeline &pipeline,
                                       const vk::Extent2D &displaySize,
                                       const std::array<float, 4>& clearColor,
                                       const std::array<float, 4>& depthStencil,
                                       const std::function<void(const vk::CommandBuffer &)> &command);
    };

} // engine
