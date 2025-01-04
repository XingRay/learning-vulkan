//
// Created by leixing on 2024/12/28.
//

#ifndef VULKANDEMO_VULKANPIPELINE_H
#define VULKANDEMO_VULKANPIPELINE_H

#include "vulkan/vulkan.hpp"
#include "VulkanDevice.h"
#include "VulkanSwapchain.h"
#include "VulkanDescriptorSet.h"
#include "VulkanRenderPass.h"

namespace engine {

    class VulkanPipeline {
    private:
        const VulkanDevice &mDevice;
        vk::Pipeline mPipeline;
        vk::PipelineLayout mPipelineLayout;

    public:
        VulkanPipeline(const VulkanDevice &vulkanDevice,
                       const VulkanSwapchain &swapchain,
                       const VulkanDescriptorSet &descriptorSet,
                       const VulkanRenderPass &renderPass,
                       const vk::ShaderModule &vertexModule,
                       const vk::ShaderModule &fragmentModule,
                       const std::vector<vk::VertexInputBindingDescription> &vertexInputBindingDescriptions,
                       const std::vector<vk::VertexInputAttributeDescription> &vertexInputAttributeDescriptions);

        ~VulkanPipeline();

        const vk::Pipeline &getPipeline() const;

        const vk::PipelineLayout &getPipelineLayout() const;
    };

} // engine

#endif //VULKANDEMO_VULKANPIPELINE_H
