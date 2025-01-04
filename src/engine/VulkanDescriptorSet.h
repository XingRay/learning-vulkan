//
// Created by leixing on 2024/12/28.
//

#ifndef VULKANDEMO_VULKANDESCRIPTORSET_H
#define VULKANDEMO_VULKANDESCRIPTORSET_H

#include "vulkan/vulkan.hpp"
#include "VulkanDevice.h"
#include "VulkanSwapchain.h"

namespace engine {

    class VulkanDescriptorSet {
    private:
        const VulkanDevice &mDevice;

        vk::DescriptorSetLayout mDescriptorSetLayout;
        vk::DescriptorPool mDescriptorPool;
        std::vector<vk::DescriptorSet> mDescriptorSets;

    public:
        VulkanDescriptorSet(const VulkanDevice &device, uint32_t frameCount/*,
                            const std::vector<vk::Buffer> &transformUniformBuffers, uint32_t transformUniformBufferSize,
                            const std::vector<vk::Buffer> &colorUniformBuffers, uint32_t colorUniformBufferSize*/);

        ~VulkanDescriptorSet();

        [[nodiscard]]
        const vk::DescriptorSetLayout &getDescriptorSetLayout() const;

        const vk::DescriptorPool &getDescriptorPool() const;

        const std::vector<vk::DescriptorSet> &getDescriptorSets() const;
    };

}

#endif //VULKANDEMO_VULKANDESCRIPTORSET_H
