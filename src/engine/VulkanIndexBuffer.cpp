//
// Created by leixing on 2024/12/30.
//

#include "VulkanIndexBuffer.h"
#include "VulkanUtil.h"
#include "Log.h"
#include "VkCheckCpp.h"

namespace engine {
    VulkanIndexBuffer::VulkanIndexBuffer(const VulkanDevice &vulkanDevice, vk::DeviceSize bufferSize) : mDevice(vulkanDevice) {
        std::tie(mIndexBuffer, mIndexBufferMemory) = VulkanUtil::createBuffer(vulkanDevice, bufferSize,
                                                                              vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer,
                                                                              vk::MemoryPropertyFlagBits::eDeviceLocal);
    }

    VulkanIndexBuffer::~VulkanIndexBuffer() {
        LOG_D("VulkanIndexBuffer::~VulkanIndexBuffer");
        const vk::Device device = mDevice.getDevice();
        device.destroy(mIndexBuffer);
        device.freeMemory(mIndexBufferMemory);
    }

    const vk::Buffer &VulkanIndexBuffer::getIndexBuffer() const {
        return mIndexBuffer;
    }

    const vk::DeviceMemory &VulkanIndexBuffer::getIndexBufferMemory() const {
        return mIndexBufferMemory;
    }

    void VulkanIndexBuffer::update(void *data, size_t size) {
        vk::Device device = mDevice.getDevice();
        void *mapped;
        CALL_VK_CPP(device.mapMemory(mIndexBufferMemory, 0, size, vk::MemoryMapFlags{}, &mapped));
        memcpy(mapped, data, size);
        device.unmapMemory(mIndexBufferMemory);
    }

    void VulkanIndexBuffer::updateByStageBuffer(const VulkanCommandPool &commandPool, const void *indices, vk::DeviceSize bufferSize) {
        vk::Device device = mDevice.getDevice();

        auto [stagingBuffer, stagingBufferMemory] = VulkanUtil::createBuffer(mDevice, bufferSize, vk::BufferUsageFlagBits::eTransferSrc,
                                                                             vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
        void *data = device.mapMemory(stagingBufferMemory, 0, bufferSize, vk::MemoryMapFlags{});
        {
            memcpy(data, indices, (size_t) bufferSize);
        }
        device.unmapMemory(stagingBufferMemory);

        commandPool.submitOneTimeCommand([&](const vk::CommandBuffer &commandBuffer) {
            VulkanUtil::recordCopyBufferCommand(commandBuffer, stagingBuffer, mIndexBuffer, bufferSize);
        });

        device.destroy(stagingBuffer);
        device.freeMemory(stagingBufferMemory);
    }
} // engine