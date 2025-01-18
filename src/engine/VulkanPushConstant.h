//
// Created by leixing on 2025/1/17.
//

#pragma once

#include "vulkan/vulkan.hpp"

namespace engine {

    class VulkanPushConstant {
    public:
        uint32_t size;
        uint32_t offset;
        vk::ShaderStageFlagBits stageFlagBits;

    public:
        VulkanPushConstant(uint32_t size, uint32_t offset, vk::ShaderStageFlagBits stageFlagBits);

        ~VulkanPushConstant();
    };

} // engine
