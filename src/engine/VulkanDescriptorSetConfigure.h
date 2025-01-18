//
// Created by leixing on 2025/1/15.
//

#pragma once

#include "engine/VulkanShaderConfigure.h"
#include "engine/VulkanDescriptorSet.h"

namespace engine {

    class VulkanDescriptorSetConfigure {
    private:
        VulkanShaderConfigure &mBuilder;

        VulkanDescriptorSet mUniformSet;

    public:

        VulkanDescriptorSetConfigure(VulkanShaderConfigure &builder);

        ~VulkanDescriptorSetConfigure();

        VulkanDescriptorSetConfigure &set(uint32_t set);

        VulkanDescriptorSetConfigure &addUniform(uint32_t binding, vk::ShaderStageFlagBits shaderStageFlagBits, uint32_t size);

        VulkanDescriptorSetConfigure &addSampler(uint32_t binding, vk::ShaderStageFlagBits shaderStageFlagBits, ImageSize imageSize);

        VulkanShaderConfigure &build();
    };

} // engine
