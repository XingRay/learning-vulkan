//
// Created by leixing on 2024/12/23.
//

#pragma once

#include <vulkan/vulkan.hpp>
#include <optional>

#include "engine/common/StringListSelector.h"
#include "engine/SwapChainSupportDetail.h"
#include "engine/QueueFamilyIndices.h"
#include "engine/VulkanPhysicalDeviceSurfaceSupport.h"
#include "engine/vulkan_wrapper/VulkanPhysicalDevice.h"

namespace engine {

    class VulkanDevice {
    private:
        vk::PhysicalDevice mPhysicalDevice;
        vk::Device mDevice;

        vk::SampleCountFlagBits mMsaaSamples = vk::SampleCountFlagBits::e1;

        uint32_t mGraphicQueueFamilyIndex;
        uint32_t mPresentQueueFamilyIndex;
        std::vector<uint32_t> mQueueFamilyIndices;

        vk::Queue mGraphicsQueue;
        vk::Queue mPresentQueue;

        vk::SurfaceCapabilitiesKHR mCapabilities;
        std::vector<vk::SurfaceFormatKHR> mFormats;
        std::vector<vk::PresentModeKHR> mPresentModes;

    public:
        VulkanDevice(const VulkanPhysicalDevice &physicalDevice,
                     const VulkanPhysicalDeviceSurfaceSupport &surfaceSupport,
                     const std::vector<std::string> &deviceExtensions,
                     const std::vector<std::string> &layers,
                     uint32_t sampleCount);

        ~VulkanDevice();

        [[nodiscard]]
        vk::PhysicalDevice getPhysicalDevice() const;

        [[nodiscard]]
        vk::Device getDevice() const;

        [[nodiscard]]
        vk::SampleCountFlagBits getMsaaSamples() const;

        [[nodiscard]]
        uint32_t getGraphicQueueFamilyIndex() const;

        [[nodiscard]]
        uint32_t getPresentQueueFamilyIndex() const;

        [[nodiscard]]
        const std::vector<uint32_t> &getQueueFamilyIndices() const;

        [[nodiscard]]
        vk::Queue getGraphicsQueue() const;

        [[nodiscard]]
        vk::Queue getPresentQueue() const;

        [[nodiscard]]
        vk::SurfaceCapabilitiesKHR getCapabilities() const;

        [[nodiscard]]
        std::vector<vk::SurfaceFormatKHR> getFormats() const;

        [[nodiscard]]
        std::vector<vk::PresentModeKHR> getPresentModes() const;

        static QueueFamilyIndices findQueueFamilies(const vk::PhysicalDevice &device, const vk::SurfaceKHR &surface);

        static vk::SampleCountFlagBits getMaxUsableSampleCount(const vk::PhysicalDevice &device);

//        static SwapChainSupportDetail querySwapChainSupported(const vk::PhysicalDevice &device, const vk::SurfaceKHR &surface);

        [[nodiscard]]
        vk::ShaderModule createShaderModule(const std::vector<char> &code) const;

    };
} // engine
