//
// Created by leixing on 2024/12/30.
//

#pragma once

#include "vulkan/vulkan.hpp"
#include "engine/vulkan_wrapper/VulkanDevice.h"
#include "engine/vulkan_wrapper/VulkanCommandPool.h"

namespace engine {

    class VulkanVertexBuffer {
    protected:
        const VulkanDevice &mDevice;
        vk::Buffer mVertexBuffer;
        vk::DeviceMemory mVertexBufferMemory;
        vk::DeviceSize mBufferSize;

    public:
        VulkanVertexBuffer(const VulkanDevice &vulkanDevice, vk::DeviceSize bufferSize);

        virtual ~VulkanVertexBuffer();

        [[nodiscard]]
        const vk::Buffer &getVertexBuffer() const;

        [[nodiscard]]
        const vk::DeviceMemory &getVertexBufferMemory() const;

        virtual void update(const void *data, size_t size) = 0;

        // 需要传入任意类型的 vector，可以是 std::vector<T> 或其他类型
        template<typename T>
        void update(const std::vector<T> &data) {
            // 检查数据类型是否满足要求 (例如可以确保传入的是简单类型或者特殊类型)
            static_assert(std::is_trivially_copyable<T>::value, "T must be trivially copyable");
            update(data.data(), sizeof(T) * data.size());
        }
    };


    // Direct transfer vertex buffer
    class DirectlyTransferVertexBuffer : public VulkanVertexBuffer {
    public:
        DirectlyTransferVertexBuffer(const VulkanDevice &vulkanDevice, vk::DeviceSize bufferSize);

        void update(const void *data, size_t size) override;
    };


    // Staging transfer vertex buffer
    class StagingTransferVertexBuffer : public VulkanVertexBuffer {
    private:
        const VulkanCommandPool &mCommandPool;
    public:
        StagingTransferVertexBuffer(const VulkanDevice &vulkanDevice, const VulkanCommandPool &vulkanCommandPool, vk::DeviceSize bufferSize);

        void update(const void *data, size_t size) override;
    };


} // engine
