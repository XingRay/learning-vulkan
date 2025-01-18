//
// Created by leixing on 2025/1/15.
//

#pragma once

#include <cstdint>

namespace engine {

    enum VulkanBufferType {
        NORMAL,
        TEXTURE_SAMPLER,
    };

    class VulkanBuffer {
    private:
        uint32_t mBinding = 0;
        VulkanBufferType mType;
        uint32_t mIndex = 0;

    public:
        explicit VulkanBuffer(uint32_t binding, VulkanBufferType type, uint32_t index);

        virtual  ~VulkanBuffer();

        [[nodiscard]]
        VulkanBufferType getType() const;

        [[nodiscard]]
        uint32_t getBinding() const;

        [[nodiscard]]
        uint32_t getIndex() const;

        virtual void updateBuffer(void *data, uint32_t size) = 0;
    };

} // engine
