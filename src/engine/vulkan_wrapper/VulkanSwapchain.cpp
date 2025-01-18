//
// Created by leixing on 2024/12/24.
//

#include "VulkanSwapchain.h"
#include "engine/Log.h"
#include "engine/VulkanUtil.h"

namespace engine {
    VulkanSwapchain::VulkanSwapchain(const VulkanDevice &vulkanDevice, const VulkanSurface &vulkanSurface, uint32_t width, uint32_t height)
            : mDevice(vulkanDevice) {
        const vk::Device& device = vulkanDevice.getDevice();

        vk::SurfaceCapabilitiesKHR capabilities = vulkanDevice.getCapabilities();
        std::vector<vk::SurfaceFormatKHR> formats = vulkanDevice.getFormats();
        std::vector<vk::PresentModeKHR> presentModes = vulkanDevice.getPresentModes();

        mDisplaySize = chooseSwapExtent(capabilities, width, height);
        mSwapChainImageFormat = chooseSwapSurfaceFormat(formats);
        vk::PresentModeKHR presentMode = choosePresentMode(presentModes);

        uint32_t imageCount = capabilities.minImageCount + 1;
        // capabilities.maxImageCount == 0 表示不做限制
        if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount) {
            imageCount = capabilities.maxImageCount;
        }
        LOG_D("imageCount: %d", imageCount);

        std::vector<uint32_t> queueFamilyIndices = vulkanDevice.getQueueFamilyIndices();
        vk::SharingMode sharingMode;
        if (queueFamilyIndices.size() == 1) {
            sharingMode = vk::SharingMode::eExclusive;
        } else {
            sharingMode = vk::SharingMode::eConcurrent;
        }

        vk::SwapchainCreateInfoKHR swapchainCreateInfo = vk::SwapchainCreateInfoKHR{}
                .setSurface(vulkanSurface.getSurface())
                .setMinImageCount(imageCount)
                .setImageFormat(mSwapChainImageFormat.format)
                .setImageColorSpace(mSwapChainImageFormat.colorSpace)
                .setImageExtent(mDisplaySize)
                .setImageArrayLayers(1)
                .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
                .setImageSharingMode(sharingMode)
                .setQueueFamilyIndices(queueFamilyIndices)
                .setPreTransform(capabilities.currentTransform)
                .setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
                .setPresentMode(presentMode)
                .setClipped(vk::True)
                .setOldSwapchain(nullptr);

        mSwapChain = device.createSwapchainKHR(swapchainCreateInfo);
        mDisplayImages = device.getSwapchainImagesKHR(mSwapChain);

        mDisplayImageViews.resize(mDisplayImages.size());

        for (int i = 0; i < mDisplayImages.size(); i++) {
            mDisplayImageViews[i] = VulkanUtil::createImageView(device, mDisplayImages[i], mSwapChainImageFormat.format, vk::ImageAspectFlagBits::eColor, 1);
        }
    }

    VulkanSwapchain::~VulkanSwapchain() {
        LOG_D("VulkanSwapchain::~VulkanSwapchain");
        vk::Device device = mDevice.getDevice();
        // 通过 getSwapchainImagesKHR 获取的对象, 不是create的对象,不需要destroy, swapchain 会自动销毁
//        for (const auto &displayImage: mDisplayImages) {
//            device.destroy(displayImage);
//        }

        for (const auto &imageView: mDisplayImageViews) {
            device.destroy(imageView);
        }

        device.destroy(mSwapChain);
    }

    const vk::SwapchainKHR &VulkanSwapchain::getSwapChain() const {
        return mSwapChain;
    }

    vk::Format VulkanSwapchain::getDisplayFormat() const {
        return mSwapChainImageFormat.format;
    }

    vk::SurfaceFormatKHR VulkanSwapchain::getSurfaceFormat() const {
        return mSwapChainImageFormat;
    }

    uint32_t VulkanSwapchain::getImageCount() const {
        return mDisplayImages.size();
    }

    vk::Extent2D VulkanSwapchain::getDisplaySize() const {
        return mDisplaySize;
    }

    const std::vector<vk::Image> &VulkanSwapchain::getDisplayImages() const {
        return mDisplayImages;
    }

    const std::vector<vk::ImageView> &VulkanSwapchain::getDisplayImageViews() const {
        return mDisplayImageViews;
    }

    vk::SurfaceFormatKHR VulkanSwapchain::chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &availableFormats) {
        for (const auto &availableFormat: availableFormats) {
            if (availableFormat.format == vk::Format::eR8G8B8A8Srgb && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
                return availableFormat;
            }

//            if (availableFormat.format == vk::Format::eR8G8B8A8Unorm && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
//                return availableFormat;
//            }
        }

        return availableFormats[0];
    }

    vk::PresentModeKHR VulkanSwapchain::choosePresentMode(const std::vector<vk::PresentModeKHR> &availablePresentModes) {
        for (const auto &availablePresentMode: availablePresentModes) {
            if (availablePresentMode == vk::PresentModeKHR::eMailbox) {
                return availablePresentMode;
            }
        }
        return vk::PresentModeKHR::eFifo;
    }

    vk::Extent2D VulkanSwapchain::chooseSwapExtent(const vk::SurfaceCapabilitiesKHR &capability, uint32_t width, uint32_t height) {
        if (capability.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
            return capability.currentExtent;
        }

        return vk::Extent2D{
                std::clamp(width, capability.minImageExtent.width, capability.maxImageExtent.width),
                std::clamp(height, capability.minImageExtent.height, capability.maxImageExtent.height),
        };
    }
}
