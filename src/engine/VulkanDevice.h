//
// Created by leixing on 2024/12/23.
//

#ifndef VULKANDEMO_VULKANDEVICE_H
#define VULKANDEMO_VULKANDEVICE_H

#include <vulkan/vulkan.hpp>
#include <optional>

namespace engine {

    class SwapChainSupportDetail {
    public:
        vk::SurfaceCapabilitiesKHR capabilities;
        std::vector<vk::SurfaceFormatKHR> formats;
        std::vector<vk::PresentModeKHR> presentModes;
    };

    class QueueFamilyIndices {

    public:
        std::optional<uint32_t> graphicQueueFamilyIndex;
        std::optional<uint32_t> presentQueueFamilyIndex;

    public:
        bool isComplete();
    };

    class PhysicalDeviceCandidate {

    public:
        vk::PhysicalDevice physicalDevice;

        std::optional<uint32_t> graphicQueueFamilyIndex;
        std::optional<uint32_t> presentQueueFamilyIndex;

        SwapChainSupportDetail swapChainSupportDetail;
    };

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
        VulkanDevice(const vk::Instance &instance,
                     const vk::SurfaceKHR &surface,
                     const std::vector<const char *> &layers,
                     const std::vector<const char *> &deviceExtensions);

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

        static std::pair<int32_t, std::unique_ptr<PhysicalDeviceCandidate>> calcDeviceSuitable(const vk::PhysicalDevice &device,
                                                                                               const std::vector<const char *> &requiredDeviceExtensions,
                                                                                               const vk::SurfaceKHR &surface);

        static QueueFamilyIndices findQueueFamilies(const vk::PhysicalDevice &device, const vk::SurfaceKHR &surface);

        static vk::SampleCountFlagBits getMaxUsableSampleCount(const vk::PhysicalDevice &device);

        static bool isDeviceSupportedRequiredDeviceExtensions(const vk::PhysicalDevice &device,
                                                              const std::vector<const char *> &requiredDeviceExtensions);

        static SwapChainSupportDetail querySwapChainSupported(const vk::PhysicalDevice &device, const vk::SurfaceKHR &surface);

        static void printPhysicalDeviceInfo(const vk::PhysicalDevice &physicalDevice, const vk::SurfaceKHR &surface);

        static std::string sampleCountFlagsToString(vk::SampleCountFlags flags);

        static std::string formatDeviceSize(vk::DeviceSize size);

        vk::ShaderModule createShaderModule(const std::vector<char> &code);

    };
} // engine

#endif //VULKANDEMO_VULKANDEVICE_H
