//
// Created by leixing on 2025/1/7.
//

#pragma once

#include "vulkan/vulkan.hpp"
#include "engine/vulkan_wrapper/VulkanDevice.h"
#include "engine/vulkan_wrapper/VulkanCommandPool.h"

#include "engine/vulkan_wrapper/VulkanBuffer.h"

namespace engine {

    class VulkanSamplerBuffer : public VulkanBuffer {
    private:
        const VulkanDevice &mDevice;
        const VulkanCommandPool &mCommandPool;

        const uint32_t mWidth;
        const uint32_t mHeight;
        const uint32_t mChannels;

        vk::DeviceSize mImageSize;
        uint32_t mMipLevels;

        vk::Image mTextureImage;
        vk::DeviceMemory mTextureImageMemory;

        vk::ImageView mTextureImageView;
        vk::Sampler mTextureSampler;

    public:

        VulkanSamplerBuffer(const VulkanDevice &vulkanDevice, const VulkanCommandPool &commandPool,
                            uint32_t width, uint32_t height, uint32_t channels,
                            uint32_t binding, uint32_t index);

        ~VulkanSamplerBuffer() override;

        [[nodiscard]]
        const vk::ImageView &getTextureImageView() const;

        [[nodiscard]]
        const vk::Sampler &getTextureSampler() const;

        void updateBuffer(void *data, uint32_t size) override;

    private:

        void generateMipmaps(vk::Image image, vk::Format imageFormat, uint32_t textureWidth, uint32_t textureHeight, uint32_t mipLevels);

        void copyBufferToImage(vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height);
    };

} // engine
