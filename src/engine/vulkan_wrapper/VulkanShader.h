//
// Created by leixing on 2025/1/5.
//

#pragma once

#include "vulkan/vulkan.hpp"

#include "engine/vulkan_wrapper/VulkanDevice.h"
#include "engine/vulkan_wrapper/VulkanCommandPool.h"
#include "engine/ShaderFormat.h"
#include "engine/ImageSize.h"
#include "engine/VulkanDescriptorSet.h"
#include "engine/VulkanVertex.h"
#include "engine/vulkan_wrapper/VulkanBuffer.h"
#include "engine/VulkanPushConstant.h"


namespace engine {
    class VulkanCommandPool;

    class VulkanShader {
    private:
        const VulkanDevice &mVulkanDevice;

        vk::ShaderModule mVertexShaderModule;
        vk::ShaderModule mFragmentShaderModule;

        std::vector<vk::VertexInputBindingDescription> mVertexDescriptions;
        std::vector<vk::VertexInputAttributeDescription> mVertexInputAttributeDescriptions;

        // uniform buffer/ texture sampler / storage buffer
        std::vector<vk::DescriptorSetLayout> mDescriptorSetLayouts;
        std::vector<std::vector<vk::DescriptorSet>> mDescriptorSets;
        vk::DescriptorPool mDescriptorPool;
        std::vector<std::vector<std::vector<std::unique_ptr<VulkanBuffer>>>> mBuffers;

        // push constant
        std::vector<vk::PushConstantRange> mPushConstantRanges;
        std::vector<std::vector<uint8_t>> mPushConstantDataList;

    public:
        explicit VulkanShader(const VulkanDevice &vulkanDevice,
                              const VulkanCommandPool &commandPool,
                              uint32_t frameCount,
                              const std::vector<char> &vertexShaderCode,
                              const std::vector<char> &fragmentShaderCode,
                              const std::vector<VulkanVertex> &vertices,
                              const std::vector<VulkanDescriptorSet> &descriptorSets,
                              const std::vector<VulkanPushConstant> &pushConstants);

        ~VulkanShader();

        [[nodiscard]]
        const vk::ShaderModule &getVertexShaderModule() const;

        [[nodiscard]]
        const vk::ShaderModule &getFragmentShaderModule() const;

        [[nodiscard]]
        const std::vector<vk::VertexInputBindingDescription> &getVertexDescriptions() const;

        [[nodiscard]]
        const std::vector<vk::VertexInputAttributeDescription> &getVertexInputAttributeDescriptions() const;

        [[nodiscard]]
        const std::vector<vk::DescriptorSetLayout> &getDescriptorSetLayouts() const;

        [[nodiscard]]
        const std::vector<vk::DescriptorSet> &getDescriptorSets(uint32_t frameIndex) const;


        [[nodiscard]]
        const std::vector<vk::PushConstantRange> &getPushConstantRanges() const;

        [[nodiscard]]
        const std::vector<std::vector<uint8_t>>& getPushConstantDataList() const;

        void updateBuffer(uint32_t frameIndex, uint32_t set, uint32_t binding, void *data, uint32_t size);

        void updatePushConstant(uint32_t index, const void *data);
    };

} // engine
