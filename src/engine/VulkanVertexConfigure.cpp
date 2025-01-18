//
// Created by leixing on 2025/1/14.
//

#include "engine/VulkanVertexConfigure.h"
#include "engine/VulkanUtil.h"

namespace engine {

    VulkanVertexConfigure::VulkanVertexConfigure(VulkanShaderConfigure &builder)
            : mBuilder(builder), mSize(0), mBinding(0), mAttributes({}) {

    }

    VulkanVertexConfigure &VulkanVertexConfigure::size(uint32_t size) {
        mSize = size;
        return *this;
    }

    VulkanVertexConfigure &VulkanVertexConfigure::binding(uint32_t binding) {
        mBinding = binding;
        return *this;
    }

    VulkanVertexConfigure &VulkanVertexConfigure::addAttribute(ShaderFormat format) {
        return addAttribute(ShaderFormatHelper::toVkFormat(format));
    }

    VulkanVertexConfigure &VulkanVertexConfigure::addAttribute(vk::Format format) {
        addAttribute(mCurrentLocation + 1, mBinding, format, mCurrentOffset);
        return *this;
    }

    VulkanVertexConfigure &VulkanVertexConfigure::addAttribute(uint32_t location, uint32_t binding, ShaderFormat format, uint32_t offset) {
        return addAttribute(location, binding, ShaderFormatHelper::toVkFormat(format), offset);
    }

    VulkanVertexConfigure &VulkanVertexConfigure::addAttribute(uint32_t location, uint32_t binding, vk::Format format, uint32_t offset) {
        mCurrentLocation = location;

        mAttributes.push_back(VulkanVertexAttribute{binding, location, format, offset});

        mCurrentOffset += VulkanUtil::getFormatSize(format);

        return *this;
    }

    VulkanVertex VulkanVertexConfigure::buildVertex() {
        return VulkanVertex{mBinding, mSize, mAttributes};
    }

    VulkanShaderConfigure &VulkanVertexConfigure::build() {
        mBuilder.addVertex(buildVertex());
        return mBuilder;
    }

} // engine