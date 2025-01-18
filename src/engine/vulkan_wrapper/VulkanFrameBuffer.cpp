//
// Created by leixing on 2024/12/31.
//

#include "VulkanFrameBuffer.h"
#include "engine/VulkanUtil.h"
#include "engine/Log.h"

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



        vk::Format depthFormat = VulkanUtil::findDepthFormat(vulkanDevice.getPhysicalDevice());

        std::tie(mDepthImage, mDepthDeviceMemory) = VulkanUtil::createImage(vulkanDevice.getDevice(), vulkanDevice.getPhysicalDevice().getMemoryProperties(), displaySize.width, displaySize.height, 1,
                                                                            vulkanDevice.getMsaaSamples(),
                                                                            depthFormat,
                                                                            vk::ImageTiling::eOptimal,
                                                                            vk::ImageUsageFlagBits::eDepthStencilAttachment,
                                                                            vk::MemoryPropertyFlagBits::eDeviceLocal);
        mDepthImageView = VulkanUtil::createImageView(device, mDepthImage, depthFormat, vk::ImageAspectFlagBits::eDepth, 1);

        commandPool.submitOneTimeCommand([&](const vk::CommandBuffer &commandBuffer) -> void {
            VulkanUtil::recordTransitionImageLayoutCommand(commandBuffer, mDepthImage, depthFormat, vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthStencilAttachmentOptimal, 1);
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
} // engine