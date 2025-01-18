//
// Created by leixing on 2025/1/14.
//

#pragma once

#include "engine/VulkanShaderConfigure.h"

namespace engine {

    class VulkanShaderConfigure;

    class VulkanVertexConfigure {
    private:
        VulkanShaderConfigure &mBuilder;

        uint32_t mSize;
        uint32_t mBinding;
        std::vector<VulkanVertexAttribute> mAttributes;

        uint32_t mCurrentLocation = -1;
        uint32_t mCurrentOffset = 0;

    public:
        VulkanVertexConfigure(VulkanShaderConfigure &builder);

        VulkanVertexConfigure &size(uint32_t size);

        VulkanVertexConfigure &binding(uint32_t binding);

        VulkanVertexConfigure &addAttribute(ShaderFormat format);

        VulkanVertexConfigure &addAttribute(vk::Format format);

        VulkanVertexConfigure &addAttribute(uint32_t location, uint32_t binding, ShaderFormat format, uint32_t offset);

        VulkanVertexConfigure &addAttribute(uint32_t location, uint32_t binding, vk::Format format, uint32_t offset);

        VulkanShaderConfigure &build();

        VulkanVertex buildVertex();
    };

} // engine
