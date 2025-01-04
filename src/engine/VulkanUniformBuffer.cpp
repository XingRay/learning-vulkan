//
// Created by leixing on 2024/12/31.
//

#include "VulkanUniformBuffer.h"
#include "VulkanUtil.h"
#include "Log.h"

namespace engine {
    VulkanUniformBuffer::VulkanUniformBuffer(const VulkanDevice &vulkanDevice, vk::DeviceSize bufferSize) : mDevice(vulkanDevice),mBufferSize(bufferSize) {
        LOG_D("VulkanUniformBuffer::VulkanUniformBuffer#bufferSize: %llu", bufferSize);
        std::tie(mUniformBuffer, mUniformBufferMemory) = VulkanUtil::createBuffer(vulkanDevice, bufferSize,
                                                                                  vk::BufferUsageFlagBits::eUniformBuffer,
                                                                                  vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
//        mUniformBuffersMapped = vulkanDevice.getDevice().mapMemory(mUniformBufferMemory, 0, bufferSize, vk::MemoryMapFlags{});

        vk::Result result = vulkanDevice.getDevice().mapMemory(mUniformBufferMemory, 0, bufferSize, vk::MemoryMapFlags{}, &mUniformBuffersMapped);
        if (result != vk::Result::eSuccess || mUniformBuffersMapped == nullptr) {
            throw std::runtime_error("Failed to map uniform buffer memory!");
        }

        LOG_D("Buffer memory: %p, mapped address: %p", static_cast<void*>(mUniformBufferMemory), mUniformBuffersMapped);

    }

    VulkanUniformBuffer::~VulkanUniformBuffer() {
        const vk::Device device = mDevice.getDevice();
        device.unmapMemory(mUniformBufferMemory);
        device.freeMemory(mUniformBufferMemory);
        device.destroy(mUniformBuffer);
        mUniformBuffersMapped = nullptr;
    }

    const vk::Buffer &VulkanUniformBuffer::getUniformBuffer() const {
        return mUniformBuffer;
    }

    const vk::DeviceMemory &VulkanUniformBuffer::getUniformBufferMemory() const {
        return mUniformBufferMemory;
    }

    void VulkanUniformBuffer::updateBuffer(void *data, uint32_t size) {
        if (mUniformBuffersMapped == nullptr) {
            LOG_E("Uniform buffer memory is not mapped!");
            return;
        }
        if (data == nullptr) {
            LOG_E("Input data is null!");
            return;
        }
        if (size > mBufferSize) {
            LOG_E("Data size (%u) exceeds buffer size (%d)!", size, mBufferSize);
            return;
        }

        LOG_D("mUniformBuffersMapped: %p", mUniformBuffersMapped);
        LOG_D("data: %p", data);
        LOG_D("size: %u", size);
        LOG_D("mBufferSize: %d", mBufferSize);

        memcpy(mUniformBuffersMapped, data, size);

        // Optional: Ensure data is flushed to GPU
//        vk::MappedMemoryRange memoryRange{};
//        memoryRange.setMemory(mUniformBufferMemory)
//                .setOffset(0)
//                .setSize(size);
//        mDevice.getDevice().flushMappedMemoryRanges(memoryRange);
    }

} // engine