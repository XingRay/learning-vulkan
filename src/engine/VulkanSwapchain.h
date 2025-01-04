//
// Created by leixing on 2024/12/24.
//

#ifndef VULKANDEMO_VULKANSWAPCHAIN_H
#define VULKANDEMO_VULKANSWAPCHAIN_H

#include "vulkan/vulkan.hpp"
#include "VulkanDevice.h"
#include "VulkanSurface.h"

namespace engine {
    class VulkanSwapchain {
    private://fields
        const VulkanDevice &mDevice;

        vk::SwapchainKHR mSwapChain;

        vk::Extent2D mDisplaySize;
        vk::SurfaceFormatKHR mSwapChainImageFormat;

        // array of frame buffers and views
        std::vector<vk::Image> mDisplayImages;
        std::vector<vk::ImageView> mDisplayImageViews;

    public://methods
        VulkanSwapchain(const VulkanDevice &vulkanDevice, const VulkanSurface &vulkanSurface, uint32_t width, uint32_t height);

        ~VulkanSwapchain();

        [[nodiscard]]
        const vk::SwapchainKHR &getSwapChain() const;

        [[nodiscard]]
        vk::Format getDisplayFormat() const;

        [[nodiscard]]
        vk::SurfaceFormatKHR getSurfaceFormat() const;

        [[nodiscard]]
        uint32_t getImageCount() const;

        [[nodiscard]]
        vk::Extent2D getDisplaySize() const;

        [[nodiscard]]
        const std::vector<vk::Image> &getDisplayImages() const;

        [[nodiscard]]
        const std::vector<vk::ImageView> &getDisplayImageViews() const;

    private:

        static vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &availableFormats);

        static vk::PresentModeKHR choosePresentMode(const std::vector<vk::PresentModeKHR> &availablePresentModes);

        static vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR &capability, uint32_t width, uint32_t height);
    };
}


#endif //VULKANDEMO_VULKANSWAPCHAIN_H
