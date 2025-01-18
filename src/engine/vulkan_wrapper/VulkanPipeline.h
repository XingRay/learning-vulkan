//
// Created by leixing on 2024/12/28.
//

#pragma once

#include "vulkan/vulkan.hpp"
#include "engine/vulkan_wrapper/VulkanDevice.h"
#include "engine/vulkan_wrapper/VulkanSwapchain.h"
#include "engine/vulkan_wrapper/VulkanRenderPass.h"
#include "engine/vulkan_wrapper/VulkanShader.h"

namespace engine {
    class VulkanShader;

    class VulkanPipeline {
    private:
        const VulkanDevice &mDevice;
        vk::Pipeline mPipeline;
        vk::PipelineLayout mPipelineLayout;

    public:
        VulkanPipeline(const VulkanDevice &vulkanDevice,
                       const VulkanSwapchain &swapchain,
                       const VulkanRenderPass &renderPass,
                       const VulkanShader &vertexShader);

        ~VulkanPipeline();

        [[nodiscard]]
        const vk::Pipeline &getPipeline() const;

        [[nodiscard]]
        const vk::PipelineLayout &getPipelineLayout() const;
    };

} // engine
