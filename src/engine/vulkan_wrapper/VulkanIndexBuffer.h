//
// Created by leixing on 2024/12/30.
//

#pragma once

#include "vulkan/vulkan.hpp"
#include "engine/vulkan_wrapper/VulkanDevice.h"
#include "engine/vulkan_wrapper/VulkanCommandPool.h"

namespace engine {

    class VulkanIndexBuffer {
    protected:
        const VulkanDevice &mDevice;

        uint32_t mIndicesCount;

        vk::Buffer mIndexBuffer;
        vk::DeviceMemory mIndexBufferMemory;
        vk::DeviceSize mBufferSize;

    public:
        VulkanIndexBuffer(const VulkanDevice &vulkanDevice, vk::DeviceSize bufferSize);

        virtual ~VulkanIndexBuffer();

        [[nodiscard]]
        const vk::Buffer &getIndexBuffer() const;

        [[nodiscard]]
        const vk::DeviceMemory &getIndexBufferMemory() const;

        [[nodiscard]]
        uint32_t getIndicesCount() const;

        virtual void update(std::vector<uint32_t> indices) = 0;
    };


    class DirectlyTransferIndexBuffer : public VulkanIndexBuffer {
    public:
        DirectlyTransferIndexBuffer(const VulkanDevice &vulkanDevice, vk::DeviceSize bufferSize);

        void update(std::vector<uint32_t> indices) override;
    };

    class StagingTransferIndexBuffer : public VulkanIndexBuffer {
    private:
        const VulkanCommandPool &mCommandPool;
    public:
        StagingTransferIndexBuffer(const VulkanDevice &vulkanDevice, const VulkanCommandPool &vulkanCommandPool, vk::DeviceSize bufferSize);

        void update(std::vector<uint32_t> indices) override;
    };

} // engine
