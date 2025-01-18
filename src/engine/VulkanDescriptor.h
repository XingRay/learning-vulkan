//
// Created by leixing on 2025/1/14.
//

#pragma once

#include <variant>

#include "vulkan/vulkan.hpp"
#include "engine/ImageSize.h"

namespace engine {

    class VulkanUniformData {
    public:
        uint32_t size;
    };

    class VulkanSamplerData {
    public:
        ImageSize imageSize;
    };

    class VulkanDescriptor {
    private:
        uint32_t mBinding;
        vk::DescriptorType mDescriptorType;
        uint32_t mIndex;
        uint32_t mDescriptorCount;
        vk::ShaderStageFlagBits mShaderStageFlagBits;


        std::variant<VulkanUniformData, VulkanSamplerData> mData;
    public:

        VulkanDescriptor(uint32_t binding, vk::ShaderStageFlagBits shaderStageFlagBits, VulkanUniformData data);

        VulkanDescriptor(uint32_t binding, vk::ShaderStageFlagBits shaderStageFlagBits, uint32_t index, uint32_t descriptorCount, VulkanUniformData data);

        VulkanDescriptor(uint32_t binding, vk::ShaderStageFlagBits shaderStageFlagBits, VulkanSamplerData data);

        VulkanDescriptor(uint32_t binding, vk::ShaderStageFlagBits shaderStageFlagBits, uint32_t index, uint32_t descriptorCount, VulkanSamplerData data);

        ~VulkanDescriptor();

        [[nodiscard]]
        uint32_t getBinding() const;

        [[nodiscard]]
        vk::DescriptorType getDescriptorType() const;

        uint32_t getIndex() const;

        uint32_t getDescriptorCount() const;

        vk::ShaderStageFlagBits getStageFlags() const;

        const VulkanUniformData &getUniformData() const;

        const VulkanSamplerData &getSamplerData() const;
    };

//    class VulkanUniformDescriptor : public VulkanDescriptor {
//
//    public:
//        explicit VulkanUniformDescriptor(uint32_t binding);
//
//        ~VulkanUniformDescriptor() override;
//    };
//
//    class VulkanSamplerDescriptor : public VulkanDescriptor {
//
//    public:
//
//        explicit VulkanSamplerDescriptor(uint32_t binding);
//
//        ~VulkanSamplerDescriptor() override;
//    };

} // engine
