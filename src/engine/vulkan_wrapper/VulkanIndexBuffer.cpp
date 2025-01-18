//
// Created by leixing on 2024/12/30.
//

#include "VulkanIndexBuffer.h"
#include "engine/VulkanUtil.h"
#include "engine/Log.h"
#include "engine/VkCheckCpp.h"

namespace engine {
    VulkanIndexBuffer::VulkanIndexBuffer(const VulkanDevice &vulkanDevice, vk::DeviceSize bufferSize) : mDevice(vulkanDevice), mBufferSize(bufferSize) {

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

    uint32_t VulkanIndexBuffer::getIndicesCount() const {
        return mIndicesCount;
    }


    DirectlyTransferIndexBuffer::DirectlyTransferIndexBuffer(const VulkanDevice &vulkanDevice, vk::DeviceSize bufferSize)
            : VulkanIndexBuffer(vulkanDevice, bufferSize) {
        std::tie(mIndexBuffer, mIndexBufferMemory) = VulkanUtil::createBuffer(vulkanDevice, bufferSize,
                                                                              vk::BufferUsageFlagBits::eVertexBuffer,
                                                                              vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
    }

    void DirectlyTransferIndexBuffer::update(std::vector<uint32_t> indices) {
        size_t size = indices.size() * sizeof(uint32_t);
        mIndicesCount = indices.size();

        vk::Device device = mDevice.getDevice();
        void *mapped;
        CALL_VK_CPP(device.mapMemory(mIndexBufferMemory, 0, size, vk::MemoryMapFlags{}, &mapped));
        memcpy(mapped, indices.data(), size);
        device.unmapMemory(mIndexBufferMemory);
    }


    StagingTransferIndexBuffer::StagingTransferIndexBuffer(const VulkanDevice &vulkanDevice, const VulkanCommandPool &vulkanCommandPool, vk::DeviceSize bufferSize)
            : VulkanIndexBuffer(vulkanDevice, bufferSize), mCommandPool(vulkanCommandPool) {
        std::tie(mIndexBuffer, mIndexBufferMemory) = VulkanUtil::createBuffer(vulkanDevice, bufferSize,
                                                                              vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer,
                                                                              vk::MemoryPropertyFlagBits::eDeviceLocal);
    }

    void StagingTransferIndexBuffer::update(std::vector<uint32_t> indices) {
        size_t size = indices.size() * sizeof(uint32_t);
        mIndicesCount = indices.size();

        vk::Device device = mDevice.getDevice();

        auto [stagingBuffer, stagingBufferMemory] = VulkanUtil::createBuffer(mDevice, size, vk::BufferUsageFlagBits::eTransferSrc,
                                                                             vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
        void *data = device.mapMemory(stagingBufferMemory, 0, size, vk::MemoryMapFlags{});
        {
            memcpy(data, indices.data(), size);
        }
        device.unmapMemory(stagingBufferMemory);

        mCommandPool.submitOneTimeCommand([&](const vk::CommandBuffer &commandBuffer) {
            VulkanUtil::recordCopyBufferCommand(commandBuffer, stagingBuffer, mIndexBuffer, size);
        });

        device.destroy(stagingBuffer);
        device.freeMemory(stagingBufferMemory);
    }
} // engine