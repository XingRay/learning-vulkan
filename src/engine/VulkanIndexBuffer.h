//
// Created by leixing on 2024/12/30.
//

#ifndef VULKANDEMO_VULKANINDEXBUFFER_H
#define VULKANDEMO_VULKANINDEXBUFFER_H

#include "vulkan/vulkan.hpp"
#include "VulkanDevice.h"
#include "VulkanCommandPool.h"

namespace engine {

    class VulkanIndexBuffer {
    private:
        const VulkanDevice &mDevice;
        vk::Buffer mIndexBuffer;
        vk::DeviceMemory mIndexBufferMemory;

    public:
        VulkanIndexBuffer(const VulkanDevice &vulkanDevice, vk::DeviceSize bufferSize);

        ~VulkanIndexBuffer();

        const vk::Buffer &getIndexBuffer() const;

        const vk::DeviceMemory &getIndexBufferMemory() const;

        void update(void *data, size_t size);

        void updateByStageBuffer(const VulkanCommandPool &commandPool, const void *indices, vk::DeviceSize bufferSize);
    };

} // engine

#endif //VULKANDEMO_VULKANINDEXBUFFER_H
