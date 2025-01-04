//
// Created by leixing on 2024/12/31.
//

#include "VulkanFrameBuffer.h"
#include "VulkanUtil.h"
#include "Log.h"

namespace engine {
    VulkanFrameBuffer::VulkanFrameBuffer(const VulkanDevice &vulkanDevice, const VulkanSwapchain &vulkanSwapchain, const VulkanRenderPass &vulkanRenderPass, const VulkanCommandPool &commandPool)
            : mDevice(vulkanDevice) {

        const vk::Device device = vulkanDevice.getDevice();
        vk::Format colorFormat = vulkanSwapchain.getDisplayFormat();
        vk::Extent2D displaySize = vulkanSwapchain.getDisplaySize();

        std::tie(mColorImage, mColorDeviceMemory) = VulkanUtil::createImage(vulkanDevice.getDevice(), vulkanDevice.getPhysicalDevice().getMemoryProperties(), displaySize.width, displaySize.height, 1,
                                                                            vulkanDevice.getMsaaSamples(), colorFormat,
                                                                            vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eTransientAttachment | vk::ImageUsageFlagBits::eColorAttachment,
                                                                            vk::MemoryPropertyFlagBits::eDeviceLocal);
        mColorImageView = VulkanUtil::createImageView(device, mColorImage, colorFormat, vk::ImageAspectFlagBits::eColor, 1);

        std::vector<vk::Format> candidate;
//        vk::ImageTiling imageTiling;
//        vk::FormatFeatureFlags formatFeatureFlags;
        vk::Format depthFormat = findDepthFormat(vulkanDevice.getPhysicalDevice());

        std::tie(mDepthImage, mDepthDeviceMemory) = VulkanUtil::createImage(vulkanDevice.getDevice(), vulkanDevice.getPhysicalDevice().getMemoryProperties(), displaySize.width, displaySize.height, 1,
                                                                            vulkanDevice.getMsaaSamples(),
                                                                            depthFormat,
                                                                            vk::ImageTiling::eOptimal,
                                                                            vk::ImageUsageFlagBits::eDepthStencilAttachment,
                                                                            vk::MemoryPropertyFlagBits::eDeviceLocal);
        mDepthImageView = VulkanUtil::createImageView(device, mDepthImage, depthFormat, vk::ImageAspectFlagBits::eDepth, 1);

        commandPool.submitOneTimeCommand([&](const vk::CommandBuffer &commandBuffer) -> void {
            recordTransitionImageLayoutCommand(commandBuffer, mDepthImage, depthFormat, vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthStencilAttachmentOptimal, 1);
        });


        mFrameBuffers.resize(vulkanSwapchain.getImageCount());
        std::vector<vk::ImageView> imageViews = vulkanSwapchain.getDisplayImageViews();

        for (int i = 0; i < vulkanSwapchain.getImageCount(); i++) {
            std::array<vk::ImageView, 3> attachments = {
                    mColorImageView,
                    mDepthImageView,
                    imageViews[i],
            };

            vk::FramebufferCreateInfo framebufferCreateInfo{};
            framebufferCreateInfo.setRenderPass(vulkanRenderPass.getRenderPass())
                    .setAttachments(attachments)
                    .setWidth(displaySize.width)
                    .setHeight(displaySize.height)
                    .setLayers(1);

            mFrameBuffers[i] = device.createFramebuffer(framebufferCreateInfo);
        }
    }

    VulkanFrameBuffer::~VulkanFrameBuffer() {
        LOG_D("VulkanFrameBuffer::~VulkanFrameBuffer()");
        vk::Device device = mDevice.getDevice();

        for (const auto &frameBuffer: mFrameBuffers) {
            device.destroy(frameBuffer);
        }

        device.destroy(mDepthImageView);
        device.destroy(mDepthImage);
        device.freeMemory(mDepthDeviceMemory);

        device.destroy(mColorImageView);
        device.destroy(mColorImage);
        device.freeMemory(mColorDeviceMemory);
    }

    const std::vector<vk::Framebuffer> &VulkanFrameBuffer::getFrameBuffers() const {
        return mFrameBuffers;
    }

    vk::Format VulkanFrameBuffer::findDepthFormat(const vk::PhysicalDevice &physicalDevice) {
        return findSupportedFormat(
                physicalDevice,
                {vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint},
                vk::ImageTiling::eOptimal,
                vk::FormatFeatureFlagBits::eDepthStencilAttachment
        );
    }

    vk::Format VulkanFrameBuffer::findSupportedFormat(const vk::PhysicalDevice &physicalDevice, const std::vector<vk::Format> &candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features) {
        for (const auto &format: candidates) {
            vk::FormatProperties properties = physicalDevice.getFormatProperties(format);
            if (tiling == vk::ImageTiling::eLinear && (properties.linearTilingFeatures & features) == features) {
                return format;
            } else if (tiling == vk::ImageTiling::eOptimal && (properties.optimalTilingFeatures & features) == features) {
                return format;
            }
        }

        throw std::runtime_error("failed to find supported format !");
    }

    void VulkanFrameBuffer::recordTransitionImageLayoutCommand(const vk::CommandBuffer &commandBuffer,
                                                               vk::Image image,
                                                               vk::Format format,
                                                               vk::ImageLayout oldImageLayout,
                                                               vk::ImageLayout newImageLayout,
                                                               uint32_t mipLevels) {

        vk::ImageSubresourceRange imageSubresourceRange;
        imageSubresourceRange
                .setBaseMipLevel(0)
                .setLevelCount(mipLevels)
                .setBaseArrayLayer(0)
                .setLayerCount(1);

        // 注意这里一定要是 vk::ImageLayout::eDepthStencilAttachmentOptimal ， 写成了 vk::ImageLayout::eStencilAttachmentOptimal 后面会报警告
        if (newImageLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal) {
            if (hasStencilComponent(format)) {
                imageSubresourceRange.setAspectMask(vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil);
            } else {
                imageSubresourceRange.setAspectMask(vk::ImageAspectFlagBits::eDepth);
            }
        } else {
            imageSubresourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor);
        }


        vk::ImageMemoryBarrier imageMemoryBarrier;
        imageMemoryBarrier.setOldLayout(oldImageLayout)
                .setNewLayout(newImageLayout)
                .setSrcQueueFamilyIndex(vk::QueueFamilyIgnored)
                .setDstQueueFamilyIndex(vk::QueueFamilyIgnored)
                .setImage(image)
                .setSubresourceRange(imageSubresourceRange)
                .setSrcAccessMask(vk::AccessFlags{})
                .setDstAccessMask(vk::AccessFlags{});

        vk::PipelineStageFlags sourceStage;
        vk::PipelineStageFlags destinationStage;

        if (oldImageLayout == vk::ImageLayout::eUndefined && newImageLayout == vk::ImageLayout::eTransferDstOptimal) {
            imageMemoryBarrier.setSrcAccessMask(static_cast<vk::AccessFlags>(0))
                    .setDstAccessMask(vk::AccessFlagBits::eTransferWrite);

            sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
            destinationStage = vk::PipelineStageFlagBits::eTransfer;
        } else if (oldImageLayout == vk::ImageLayout::eTransferDstOptimal && newImageLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {
            imageMemoryBarrier.setSrcAccessMask(vk::AccessFlagBits::eTransferWrite)
                    .setDstAccessMask(vk::AccessFlagBits::eShaderRead);

            sourceStage = vk::PipelineStageFlagBits::eTransfer;
            destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
        } else if (oldImageLayout == vk::ImageLayout::eUndefined && newImageLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal) {
            imageMemoryBarrier.setSrcAccessMask(vk::AccessFlags{})
                    .setDstAccessMask(vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite);

            sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
            destinationStage = vk::PipelineStageFlagBits::eEarlyFragmentTests;
        } else {
            throw std::runtime_error("unsupported layout transition!");
        }

        commandBuffer.pipelineBarrier(sourceStage,
                                      destinationStage,
                                      vk::DependencyFlags{},
                // 内存屏障
                                      {},
                // 缓冲区内存屏障
                                      {},
                // 图像内存屏障
                                      {imageMemoryBarrier});


    }

    bool VulkanFrameBuffer::hasStencilComponent(vk::Format format) {
        return format == vk::Format::eD32SfloatS8Uint || format == vk::Format::eD24UnormS8Uint;
    }

} // engine