//
// Created by leixing on 2024/12/31.
//

#ifndef VULKANDEMO_VULKANSYNCOBJECT_H
#define VULKANDEMO_VULKANSYNCOBJECT_H

#include "vulkan/vulkan.hpp"
#include "VulkanDevice.h"
#include "Log.h"

namespace engine {

    class VulkanSyncObject {
    private:
        const VulkanDevice &mDevice;

        std::vector<vk::Semaphore> mImageAvailableSemaphores;

        std::vector<vk::Semaphore> mRenderFinishedSemaphores;

        std::vector<vk::Fence> mFences;

    public:
        explicit VulkanSyncObject(const VulkanDevice &vulkanDevice, uint32_t count);

        ~VulkanSyncObject();

        [[nodiscard]]
        const vk::Semaphore &getImageAvailableSemaphore(uint32_t index) const;

        [[nodiscard]]
        const vk::Semaphore &getRenderFinishedSemaphore(uint32_t index) const;

        [[nodiscard]]
        vk::Fence getFence(uint32_t index) const;

        vk::Result resetFence(uint32_t index);

        vk::Result waitFence(uint32_t index);
    };

} // engine

#endif //VULKANDEMO_VULKANSYNCOBJECT_H
