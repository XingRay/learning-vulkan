//
// Created by leixing on 2025/1/14.
//

#include "engine/VulkanDescriptor.h"

namespace engine {

    /**
     *
     * VulkanUniform
     *
     */
    VulkanDescriptor::VulkanDescriptor(uint32_t binding, vk::ShaderStageFlagBits shaderStageFlagBits, VulkanUniformData data)
            : VulkanDescriptor(binding, shaderStageFlagBits, 0, 1, data) {

    }

    VulkanDescriptor::VulkanDescriptor(uint32_t binding, vk::ShaderStageFlagBits shaderStageFlagBits, uint32_t index, uint32_t descriptorCount, VulkanUniformData data)
            : mBinding(binding), mDescriptorType(vk::DescriptorType::eUniformBuffer), mShaderStageFlagBits(shaderStageFlagBits), mIndex(index), mDescriptorCount(descriptorCount), mData(data) {

    }

    VulkanDescriptor::VulkanDescriptor(uint32_t binding, vk::ShaderStageFlagBits shaderStageFlagBits, VulkanSamplerData data)
            : VulkanDescriptor(binding, shaderStageFlagBits, 0, 1, data) {

    }

    VulkanDescriptor::VulkanDescriptor(uint32_t binding, vk::ShaderStageFlagBits shaderStageFlagBits, uint32_t index, uint32_t descriptorCount, VulkanSamplerData data)
            : mBinding(binding), mDescriptorType(vk::DescriptorType::eCombinedImageSampler), mShaderStageFlagBits(shaderStageFlagBits), mIndex(index), mDescriptorCount(descriptorCount), mData(data) {

    }

    VulkanDescriptor::~VulkanDescriptor() = default;

    uint32_t VulkanDescriptor::getBinding() const {
        return mBinding;
    }

    vk::DescriptorType VulkanDescriptor::getDescriptorType() const {
        return mDescriptorType;
    }

    uint32_t VulkanDescriptor::getIndex() const {
        return mIndex;
    }

    uint32_t VulkanDescriptor::getDescriptorCount() const {
        return mDescriptorCount;
    }

    vk::ShaderStageFlagBits VulkanDescriptor::getStageFlags() const {
        return mShaderStageFlagBits;
    }

    const VulkanUniformData &VulkanDescriptor::getUniformData() const {
        return std::get<VulkanUniformData>(mData);
    }

    const VulkanSamplerData &VulkanDescriptor::getSamplerData() const {
        return std::get<VulkanSamplerData>(mData);
    }

//    /**
//     *
//     * VulkanNormalUniform
//     *
//     */
//    VulkanUniformDescriptor::VulkanUniformDescriptor(uint32_t binding)
//            : VulkanDescriptor{binding, vk::DescriptorType::eUniformBuffer} {
//
//    }
//
//    VulkanUniformDescriptor::~VulkanUniformDescriptor() {
//
//    }
//
//
//    /**
//     *
//     * VulkanSamplerUniform
//     *
//     */
//    VulkanSamplerDescriptor::VulkanSamplerDescriptor(uint32_t binding)
//            : VulkanDescriptor{binding, vk::DescriptorType::eCombinedImageSampler} {
//
//    }
//
//    VulkanSamplerDescriptor::~VulkanSamplerDescriptor() = default;
}