//
// Created by leixing on 2025/1/16.
//

#include "VulkanBuffer.h"

namespace engine {

    VulkanBuffer::VulkanBuffer(uint32_t binding, VulkanBufferType type, uint32_t index) : mBinding(binding), mType(type), mIndex(index) {

    }

    engine::VulkanBuffer::~VulkanBuffer() = default;


    VulkanBufferType VulkanBuffer::getType() const {
        return mType;
    }

    uint32_t VulkanBuffer::getBinding() const {
        return mBinding;
    }

    uint32_t VulkanBuffer::getIndex() const {
        return mIndex;
    }

}