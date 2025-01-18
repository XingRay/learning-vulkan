//
// Created by leixing on 2025/1/7.
//

#include "VulkanSamplerBuffer.h"
#include "engine/VulkanUtil.h"

namespace engine {

    VulkanSamplerBuffer::VulkanSamplerBuffer(const VulkanDevice &vulkanDevice, const VulkanCommandPool &commandPool,
                                             uint32_t width, uint32_t height, uint32_t channels,
                                             uint32_t binding, uint32_t index)
            : mDevice(vulkanDevice), mCommandPool(commandPool),
              mWidth(width), mHeight(height), mChannels(channels),
              VulkanBuffer(binding, VulkanBufferType::TEXTURE_SAMPLER, index) {

        const vk::Device device = vulkanDevice.getDevice();

        mImageSize = width * height * channels;
        mMipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(width, height)))) + 1;
        vk::Format imageFormat = vk::Format::eR8G8B8A8Srgb;

        std::tie(mTextureImage, mTextureImageMemory) = VulkanUtil::createImage(device,
                                                                               vulkanDevice.getPhysicalDevice().getMemoryProperties(),
                                                                               width, height,
                                                                               mMipLevels,
                                                                               vk::SampleCountFlagBits::e1,
                                                                               imageFormat,
                                                                               vk::ImageTiling::eOptimal,
                                                                               vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
                                                                               vk::MemoryPropertyFlagBits::eDeviceLocal);

        mTextureImageView = VulkanUtil::createImageView(device, mTextureImage, imageFormat, vk::ImageAspectFlagBits::eColor, mMipLevels);

        commandPool.submitOneTimeCommand([&](const vk::CommandBuffer &commandBuffer) -> void {
            VulkanUtil::recordTransitionImageLayoutCommand(commandBuffer, mTextureImage, imageFormat, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal, mMipLevels);
        });


        vk::PhysicalDeviceProperties properties = mDevice.getPhysicalDevice().getProperties();

        vk::SamplerCreateInfo samplerCreateInfo;
        samplerCreateInfo
                .setMagFilter(vk::Filter::eLinear)
                .setMinFilter(vk::Filter::eLinear)
                .setAddressModeU(vk::SamplerAddressMode::eRepeat)
                .setAddressModeV(vk::SamplerAddressMode::eRepeat)
                .setAddressModeW(vk::SamplerAddressMode::eRepeat)
                .setAnisotropyEnable(vk::True)
                .setMaxAnisotropy(properties.limits.maxSamplerAnisotropy)
                .setBorderColor(vk::BorderColor::eIntOpaqueBlack)
                        // 是否使用不归一化的坐标(x:[0~width], y:[0~height])， 通常使用归一化坐标(x:[0~1.0], y:[0~1.0])， 所以设置为 false，
                .setUnnormalizedCoordinates(vk::False)
                .setCompareEnable(vk::False)
                .setCompareOp(vk::CompareOp::eAlways)
                .setMipmapMode(vk::SamplerMipmapMode::eLinear)
                .setMipLodBias(0.0f)
                .setMinLod(0.0f)
                        // 测试： 强制使用高等级 mipmap （更模糊）
//            .setMinLod(static_cast<float >(mMipLevels / 2))
                .setMaxLod(static_cast<float >(mMipLevels));

        mTextureSampler = device.createSampler(samplerCreateInfo);
    }

    VulkanSamplerBuffer::~VulkanSamplerBuffer() {
        vk::Device device = mDevice.getDevice();

        device.destroy(mTextureSampler);
        device.destroy(mTextureImageView);
        device.destroy(mTextureImage);
        device.unmapMemory(mTextureImageMemory);
    }

    const vk::ImageView &VulkanSamplerBuffer::getTextureImageView() const {
        return mTextureImageView;
    }

    const vk::Sampler &VulkanSamplerBuffer::getTextureSampler() const {
        return mTextureSampler;
    }

    void VulkanSamplerBuffer::updateBuffer(void *data, uint32_t size) {
        auto [stagingBuffer, stagingBufferMemory] = VulkanUtil::createBuffer(mDevice, mImageSize, vk::BufferUsageFlagBits::eTransferSrc,
                                                                             vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
        void *mappedMemory = mDevice.getDevice().mapMemory(stagingBufferMemory, 0, mImageSize, vk::MemoryMapFlags{});
        memcpy(mappedMemory, data, mImageSize);
        mDevice.getDevice().unmapMemory(stagingBufferMemory);

        copyBufferToImage(stagingBuffer, mTextureImage, mWidth, mHeight);

        mDevice.getDevice().destroy(stagingBuffer);
        mDevice.getDevice().freeMemory(stagingBufferMemory);

        generateMipmaps(mTextureImage, vk::Format::eR8G8B8A8Srgb, mWidth, mHeight, mMipLevels);
    }

    void VulkanSamplerBuffer::generateMipmaps(vk::Image image, vk::Format imageFormat, uint32_t textureWidth, uint32_t textureHeight, uint32_t mipLevels) {
        vk::FormatProperties formatProperties = mDevice.getPhysicalDevice().getFormatProperties(imageFormat);
        if (!(formatProperties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eSampledImageFilterLinear)) {
            throw std::runtime_error("texture image format does not support linear blitting!");
        }

        mCommandPool.submitOneTimeCommand([&](const vk::CommandBuffer &commandBuffer) {
            vk::ImageMemoryBarrier imageMemoryBarrier;
            imageMemoryBarrier.setImage(image)
                    .setSrcQueueFamilyIndex(vk::QueueFamilyIgnored)
                    .setDstQueueFamilyIndex(vk::QueueFamilyIgnored);

            vk::ImageSubresourceRange &subresourceRange = imageMemoryBarrier.subresourceRange;
            subresourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor)
                    .setBaseArrayLayer(0)
                    .setLayerCount(1)
                    .setLevelCount(1);

            uint32_t mipWidth = textureWidth;
            uint32_t mipHeight = textureHeight;

            for (int i = 1; i < mipLevels; i++) {
                subresourceRange.setBaseMipLevel(i - 1);
                imageMemoryBarrier.setOldLayout(vk::ImageLayout::eTransferDstOptimal)
                        .setNewLayout(vk::ImageLayout::eTransferSrcOptimal)
                        .setSrcAccessMask(vk::AccessFlagBits::eTransferWrite)
                        .setDstAccessMask(vk::AccessFlagBits::eTransferRead);

                commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTransfer,
                                              static_cast<vk::DependencyFlags>(0),
                                              {},
                                              {},
                                              {imageMemoryBarrier});

                vk::ImageBlit imageBlit;

                std::array<vk::Offset3D, 2> &srcOffsets = imageBlit.srcOffsets;
                srcOffsets[0] = vk::Offset3D{0, 0, 0};
                srcOffsets[1] = vk::Offset3D{static_cast<int32_t>(mipWidth), static_cast<int32_t>(mipHeight), 1};

                vk::ImageSubresourceLayers &srcImageSubresourceLayers = imageBlit.srcSubresource;
                srcImageSubresourceLayers.setAspectMask(vk::ImageAspectFlagBits::eColor)
                        .setMipLevel(i - 1)
                        .setBaseArrayLayer(0)
                        .setLayerCount(1);

                std::array<vk::Offset3D, 2> &dstOffsets = imageBlit.dstOffsets;
                dstOffsets[0] = vk::Offset3D{0, 0, 0};
                dstOffsets[1] = vk::Offset3D{static_cast<int32_t>(mipWidth > 1 ? mipWidth / 2 : 1), static_cast<int32_t>(mipHeight > 1 ? mipHeight / 2 : 1), 1};

                vk::ImageSubresourceLayers &dstImageSubresourceLayers = imageBlit.dstSubresource;
                dstImageSubresourceLayers.setAspectMask(vk::ImageAspectFlagBits::eColor)
                        .setMipLevel(i)
                        .setBaseArrayLayer(0)
                        .setLayerCount(1);

                commandBuffer.blitImage(image, vk::ImageLayout::eTransferSrcOptimal, image, vk::ImageLayout::eTransferDstOptimal, {imageBlit}, vk::Filter::eLinear);

                imageMemoryBarrier.setOldLayout(vk::ImageLayout::eTransferSrcOptimal)
                        .setNewLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
                        .setSrcAccessMask(vk::AccessFlagBits::eTransferWrite)
                        .setDstAccessMask(vk::AccessFlagBits::eShaderRead);

                commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader,
                                              static_cast<vk::DependencyFlags>(0),
                                              {},
                                              {},
                                              {imageMemoryBarrier});

                if (mipWidth > 1) {
                    mipWidth /= 2;
                }

                if (mipHeight > 1) {
                    mipHeight /= 2;
                }
            }

            subresourceRange.setBaseMipLevel(mipLevels - 1);
            imageMemoryBarrier.setOldLayout(vk::ImageLayout::eTransferDstOptimal)
                    .setNewLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
                    .setSrcAccessMask(vk::AccessFlagBits::eTransferWrite)
                    .setDstAccessMask(vk::AccessFlagBits::eShaderRead);

            commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader,
                                          static_cast<vk::DependencyFlags>(0),
                                          {},
                                          {},
                                          {imageMemoryBarrier});
        });
    }

    void VulkanSamplerBuffer::copyBufferToImage(vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height) {
        mCommandPool.submitOneTimeCommand([&](const vk::CommandBuffer &commandBuffer) {
            vk::ImageSubresourceLayers imageSubresourceLayers;
            imageSubresourceLayers.setAspectMask(vk::ImageAspectFlagBits::eColor)
                    .setMipLevel(0)
                    .setBaseArrayLayer(0)
                    .setLayerCount(1);

            vk::Offset3D offset{0, 0, 0};
            vk::Extent3D extent{width, height, 1};

            vk::BufferImageCopy bufferImageCopy;
            bufferImageCopy.setBufferOffset(0)
                    .setBufferRowLength(0)
                    .setBufferImageHeight(0)
                    .setImageSubresource(imageSubresourceLayers)
                    .setImageOffset(offset)
                    .setImageExtent(extent);

            commandBuffer.copyBufferToImage(buffer, image, vk::ImageLayout::eTransferDstOptimal, {bufferImageCopy});
        });
    }
} // engine