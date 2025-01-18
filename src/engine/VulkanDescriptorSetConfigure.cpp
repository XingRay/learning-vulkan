//
// Created by leixing on 2025/1/15.
//

#include "VulkanDescriptorSetConfigure.h"

namespace engine {

    VulkanDescriptorSetConfigure::VulkanDescriptorSetConfigure(VulkanShaderConfigure &builder)
            : mBuilder(builder), mUniformSet({}) {

    }

    VulkanDescriptorSetConfigure::~VulkanDescriptorSetConfigure() = default;

    VulkanDescriptorSetConfigure &VulkanDescriptorSetConfigure::set(uint32_t set) {
        mUniformSet.set = set;
        return *this;
    }

    VulkanDescriptorSetConfigure &VulkanDescriptorSetConfigure::addUniform(uint32_t binding, vk::ShaderStageFlagBits shaderStageFlagBits, uint32_t size) {
        VulkanUniformData data{size};
        mUniformSet.descriptors.emplace_back(binding, shaderStageFlagBits, data);
        return *this;
    }

    VulkanDescriptorSetConfigure &VulkanDescriptorSetConfigure::addSampler(uint32_t binding, vk::ShaderStageFlagBits shaderStageFlagBits, ImageSize imageSize) {
        VulkanSamplerData data{imageSize};
        mUniformSet.descriptors.emplace_back(binding, shaderStageFlagBits, data);
        return *this;
    }

    VulkanShaderConfigure &VulkanDescriptorSetConfigure::build() {
        mBuilder.addUniformSet(mUniformSet);
        return mBuilder;
    }

} // engine