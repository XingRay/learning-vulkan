//
// Created by leixing on 2024/12/31.
//

#include "VulkanSyncObject.h"
#include "engine/Log.h"

namespace engine {
    VulkanSyncObject::VulkanSyncObject(const VulkanDevice &vulkanDevice, uint32_t count) : mDevice(vulkanDevice) {
        vk::Device device = vulkanDevice.getDevice();

        vk::SemaphoreCreateInfo semaphoreCreateInfo{};
        vk::FenceCreateInfo fenceCreateInfo{};
        // 已发出信号的状态下创建栅栏，以便第一次调用 vkWaitForFences()立即返回
        fenceCreateInfo.setFlags(vk::FenceCreateFlagBits::eSignaled);

        for (int i = 0; i < count; i++) {
            mImageAvailableSemaphores.push_back(device.createSemaphore(semaphoreCreateInfo));
            mRenderFinishedSemaphores.push_back(device.createSemaphore(semaphoreCreateInfo));
            mFences.push_back(device.createFence(fenceCreateInfo));
        }
    }

    VulkanSyncObject::~VulkanSyncObject() {
        LOG_D("VulkanSyncObject::~VulkanSyncObject");
        vk::Device device = mDevice.getDevice();
        for (const auto &semaphore: mImageAvailableSemaphores) {
            device.destroy(semaphore);
        }
        for (const auto &semaphore: mRenderFinishedSemaphores) {
            device.destroy(semaphore);
        }
        for (const auto &fence: mFences) {
            device.destroy(fence);
        }
    }

    const vk::Semaphore &VulkanSyncObject::getImageAvailableSemaphore(uint32_t index) const {
        return mImageAvailableSemaphores[index];
    }

    const vk::Semaphore &VulkanSyncObject::getRenderFinishedSemaphore(uint32_t index) const {
        return mRenderFinishedSemaphores[index];
    }

    vk::Fence VulkanSyncObject::getFence(uint32_t index) const {
        return mFences[index];
    }

    vk::Result VulkanSyncObject::resetFence(uint32_t index) {
        vk::Device device = mDevice.getDevice();
        return device.resetFences(1, &mFences[index]);
    }

    vk::Result VulkanSyncObject::waitFence(uint32_t index) {
        vk::Device device = mDevice.getDevice();
        return device.waitForFences(1, &mFences[index], vk::True, std::numeric_limits<uint64_t>::max());
    }
} // engine