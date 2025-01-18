//
// Created by leixing on 2025/1/17.
//

#include "VulkanPushConstant.h"

namespace engine {

    VulkanPushConstant::VulkanPushConstant(uint32_t size, uint32_t offset, vk::ShaderStageFlagBits stageFlagBits)
            : size(size), offset(offset), stageFlagBits(stageFlagBits) {

    }

    VulkanPushConstant::~VulkanPushConstant() = default;

} // engine