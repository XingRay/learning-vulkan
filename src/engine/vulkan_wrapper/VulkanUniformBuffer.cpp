//
// Created by leixing on 2024/12/31.
//

#include "VulkanUniformBuffer.h"
#include "engine/VulkanUtil.h"
#include "engine/Log.h"

namespace engine {
    VulkanUniformBuffer::VulkanUniformBuffer(const VulkanDevice &vulkanDevice, vk::DeviceSize bufferSize, uint32_t binding, uint32_t index)
            : mDevice(vulkanDevice), mBufferSize(bufferSize), VulkanBuffer(binding, VulkanBufferType::NORMAL, index) {
        LOG_D("VulkanUniformBuffer::VulkanUniformBuffer#bufferSize: %lu", bufferSize);
        std::tie(mUniformBuffer, mUniformBufferMemory) = VulkanUtil::createBuffer(vulkanDevice, bufferSize,
                                                                                  vk::BufferUsageFlagBits::eUniformBuffer,
                                                                                  vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
        mUniformBuffersMapped = vulkanDevice.getDevice().mapMemory(mUniformBufferMemory, 0, bufferSize, vk::MemoryMapFlags{});

        vk::Result result = vulkanDevice.getDevice().mapMemory(mUniformBufferMemory, 0, bufferSize, vk::MemoryMapFlags{}, &mUniformBuffersMapped);
        if (result != vk::Result::eSuccess || mUniformBuffersMapped == nullptr) {
            throw std::runtime_error("Failed to map uniform buffer memory!");
        }

        LOG_D("Buffer memory: %p, mapped address: %p", static_cast<void *>(mUniformBufferMemory), mUniformBuffersMapped);

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

    uint32_t VulkanUniformBuffer::getBufferSize() const {
        return mBufferSize;
    }

    uint32_t VulkanUniformBuffer::getOffset() const {
        return mOffset;
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

        memcpy(mUniformBuffersMapped, data, size);
    }

} // engine