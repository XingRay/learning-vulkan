//
// Created by leixing on 2024/12/30.
//

#include "VulkanVertexBuffer.h"
#include "VulkanUtil.h"
#include "Log.h"
#include "VkCheckCpp.h"

namespace engine {
    VulkanVertexBuffer::VulkanVertexBuffer(const VulkanDevice &vulkanDevice, vk::DeviceSize bufferSize) : mDevice(vulkanDevice) {
        std::tie(mVertexBuffer, mVertexBufferMemory) = VulkanUtil::createBuffer(vulkanDevice, bufferSize,
                                                                                vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
                                                                                vk::MemoryPropertyFlagBits::eDeviceLocal);
    }

    VulkanVertexBuffer::~VulkanVertexBuffer() {
        LOG_D("VulkanVertexBuffer::~VulkanVertexBuffer");
        const vk::Device device = mDevice.getDevice();
        device.destroy(mVertexBuffer);
        device.free(mVertexBufferMemory);
    }

    const vk::Buffer &VulkanVertexBuffer::getVertexBuffer() const {
        return mVertexBuffer;
    }

    const vk::DeviceMemory &VulkanVertexBuffer::getVertexBufferMemory() const {
        return mVertexBufferMemory;
    }


    void VulkanVertexBuffer::update(void *data, size_t size) {
        vk::Device device = mDevice.getDevice();
        void *mapped;
        CALL_VK_CPP(device.mapMemory(mVertexBufferMemory, 0, size, vk::MemoryMapFlags{}, &mapped));
        memcpy(mapped, data, size);
        device.unmapMemory(mVertexBufferMemory);
    }


    void VulkanVertexBuffer::updateByStageBuffer(const VulkanCommandPool &commandPool, const void *vertices, vk::DeviceSize bufferSize) {
        vk::Device device = mDevice.getDevice();

        auto [stagingBuffer, stagingBufferMemory] = VulkanUtil::createBuffer(mDevice, bufferSize, vk::BufferUsageFlagBits::eTransferSrc,
                                                                             vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
        void *data = device.mapMemory(stagingBufferMemory, 0, bufferSize, vk::MemoryMapFlags{});
        memcpy(data, vertices, (size_t) bufferSize);
        device.unmapMemory(stagingBufferMemory);

        commandPool.submitOneTimeCommand([&](const vk::CommandBuffer &commandBuffer) {
            VulkanUtil::recordCopyBufferCommand(commandBuffer, stagingBuffer, mVertexBuffer, bufferSize);
        });

        device.destroy(stagingBuffer);
        device.freeMemory(stagingBufferMemory);
    }

} // engine