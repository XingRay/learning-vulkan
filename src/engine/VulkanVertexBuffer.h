//
// Created by leixing on 2024/12/30.
//

#ifndef VULKANDEMO_VULKANVERTEXBUFFER_H
#define VULKANDEMO_VULKANVERTEXBUFFER_H

#include "vulkan/vulkan.hpp"
#include "VulkanDevice.h"
#include "VulkanCommandPool.h"

namespace engine {

    class VulkanVertexBuffer {
    private:
        const VulkanDevice &mDevice;
        vk::Buffer mVertexBuffer;
        vk::DeviceMemory mVertexBufferMemory;

    public:
        VulkanVertexBuffer(const VulkanDevice &vulkanDevice, vk::DeviceSize bufferSize);

        ~VulkanVertexBuffer();

        [[nodiscard]]
        const vk::Buffer &getVertexBuffer() const;

        [[nodiscard]]
        const vk::DeviceMemory &getVertexBufferMemory() const;

        void update(void *data, size_t size);

        void updateByStageBuffer(const VulkanCommandPool &commandPool, const void* vertices, vk::DeviceSize bufferSize);

    };

} // engine

#endif //VULKANDEMO_VULKANVERTEXBUFFER_H
