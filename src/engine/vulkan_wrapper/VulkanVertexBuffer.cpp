//
// Created by leixing on 2024/12/30.
//

#include "VulkanVertexBuffer.h"
#include "engine/VulkanUtil.h"
#include "engine/Log.h"
#include "engine/VkCheckCpp.h"

namespace engine {

    VulkanVertexBuffer::VulkanVertexBuffer(const VulkanDevice &vulkanDevice, vk::DeviceSize bufferSize)
            : mDevice(vulkanDevice), mBufferSize(bufferSize) {
        LOG_D("VulkanVertexBuffer::VulkanVertexBuffer");
    }

    VulkanVertexBuffer::~VulkanVertexBuffer() {
        LOG_D("VulkanVertexBuffer::~VulkanVertexBuffer");
        const vk::Device device = mDevice.getDevice();
        device.destroy(mVertexBuffer);
        device.freeMemory(mVertexBufferMemory);
    }


    const vk::Buffer &VulkanVertexBuffer::getVertexBuffer() const {
        return mVertexBuffer;
    }


    const vk::DeviceMemory &VulkanVertexBuffer::getVertexBufferMemory() const {
        return mVertexBufferMemory;
    }

    DirectlyTransferVertexBuffer::DirectlyTransferVertexBuffer(const VulkanDevice &vulkanDevice, vk::DeviceSize bufferSize)
            : VulkanVertexBuffer(vulkanDevice, bufferSize) {
        LOG_D("DirectlyTransferVertexBuffer::DirectlyTransferVertexBuffer");
        std::tie(this->mVertexBuffer, this->mVertexBufferMemory) = VulkanUtil::createBuffer(
                vulkanDevice, bufferSize,
                vk::BufferUsageFlagBits::eVertexBuffer,
                vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
    }

    void DirectlyTransferVertexBuffer::update(const void *data, size_t size) {
        LOG_D("DirectlyTransferVertexBuffer::update, data: %p,  size: %lu", data, size);
        vk::Device device = this->mDevice.getDevice();
        void *mapped;
        if (size > this->mBufferSize) {
            throw std::runtime_error("Data size exceeds buffer size.");
        }
        CALL_VK_CPP(device.mapMemory(this->mVertexBufferMemory, 0, size, vk::MemoryMapFlags{}, &mapped));
        memcpy(mapped, data, size);
        device.unmapMemory(this->mVertexBufferMemory);
    }


    StagingTransferVertexBuffer::StagingTransferVertexBuffer(const VulkanDevice &vulkanDevice, const VulkanCommandPool &vulkanCommandPool, vk::DeviceSize bufferSize)
            : VulkanVertexBuffer(vulkanDevice, bufferSize), mCommandPool(vulkanCommandPool) {
        LOG_D("StagingTransferVertexBuffer::StagingTransferVertexBuffer");
        std::tie(mVertexBuffer, mVertexBufferMemory) = VulkanUtil::createBuffer(
                vulkanDevice, bufferSize,
                vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
                vk::MemoryPropertyFlagBits::eDeviceLocal);
    }

    void StagingTransferVertexBuffer::update(const void *data, size_t size) {
        LOG_D("StagingTransferVertexBuffer::update, data: %p,  size: %lu", data, size);
        vk::Device device = this->mDevice.getDevice();
        if (size > mBufferSize) {
            throw std::runtime_error("Data size exceeds buffer size.");
        }

        // Create staging buffer
        auto [stagingBuffer, stagingBufferMemory] = VulkanUtil::createBuffer(
                mDevice, size, vk::BufferUsageFlagBits::eTransferSrc,
                vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

        // Copy data to staging buffer
        void *mapped;
        CALL_VK_CPP(device.mapMemory(stagingBufferMemory, 0, size, vk::MemoryMapFlags{}, &mapped));
        memcpy(mapped, data, size);
        device.unmapMemory(stagingBufferMemory);

        // Copy staging buffer to vertex buffer
        mCommandPool.submitOneTimeCommand([&](const vk::CommandBuffer &commandBuffer) {
            VulkanUtil::recordCopyBufferCommand(commandBuffer, stagingBuffer, mVertexBuffer, size);
        });

        // Cleanup staging buffer
        device.destroy(stagingBuffer);
        device.freeMemory(stagingBufferMemory);
    }

} // engine
