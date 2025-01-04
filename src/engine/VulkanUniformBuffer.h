//
// Created by leixing on 2024/12/31.
//

#ifndef VULKANDEMO_VULKANUNIFORMBUFFER_H
#define VULKANDEMO_VULKANUNIFORMBUFFER_H

#include "vulkan/vulkan.hpp"
#include "VulkanDevice.h"
#include "VulkanCommandPool.h"

namespace engine {

    class VulkanUniformBuffer {
    private:
        const VulkanDevice &mDevice;
        uint32_t mBufferSize;

        vk::Buffer mUniformBuffer;
        vk::DeviceMemory mUniformBufferMemory;
        void *mUniformBuffersMapped;

    public:
        VulkanUniformBuffer(const VulkanDevice &vulkanDevice, vk::DeviceSize bufferSize);

        ~VulkanUniformBuffer();

        [[nodiscard]]
        const vk::Buffer &getUniformBuffer() const;

        [[nodiscard]]
        const vk::DeviceMemory &getUniformBufferMemory() const;

        void updateBuffer(void *data, uint32_t size);
    };

} // engine

#endif //VULKANDEMO_VULKANUNIFORMBUFFER_H
