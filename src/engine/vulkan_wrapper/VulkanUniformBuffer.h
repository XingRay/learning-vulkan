//
// Created by leixing on 2024/12/31.
//

#pragma once

#include "vulkan/vulkan.hpp"
#include "engine/vulkan_wrapper/VulkanDevice.h"

#include "engine/vulkan_wrapper/VulkanBuffer.h"

namespace engine {

    class VulkanUniformBuffer : public VulkanBuffer {
    private:
        const VulkanDevice &mDevice;
        /**
         * 指定缓冲区的范围（以字节为单位）。
         */
        uint32_t mBufferSize;

        /**
         *  指定缓冲区的起始偏移量（以字节为单位）。
         */
        uint32_t mOffset = 0;

        vk::Buffer mUniformBuffer;
        vk::DeviceMemory mUniformBufferMemory;
        void *mUniformBuffersMapped;

    public:
        VulkanUniformBuffer(const VulkanDevice &vulkanDevice, vk::DeviceSize bufferSize, uint32_t binding, uint32_t index);

        ~VulkanUniformBuffer() override;

        [[nodiscard]]
        const vk::Buffer &getUniformBuffer() const;

        [[nodiscard]]
        const vk::DeviceMemory &getUniformBufferMemory() const;

        [[nodiscard]]
        uint32_t getBufferSize() const;

        [[nodiscard]]
        uint32_t getOffset() const;

        void updateBuffer(void *data, uint32_t size) override;
    };

} // engine
