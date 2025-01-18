//
// Created by leixing on 2025/1/9.
//

#pragma once

#include "vulkan/vulkan.hpp"

#include "engine/vulkan_wrapper/VulkanInstance.h"
#include "engine/vulkan_wrapper/VulkanSurface.h"
#include "engine/vulkan_wrapper/VulkanPhysicalDevice.h"
#include "engine/VulkanPhysicalDeviceCandidate.h"

#include "engine/VulkanPhysicalDeviceScoreConfig.h"
#include "engine/VulkanPhysicalDeviceScoreCalculator.h"

namespace engine {

    class VulkanPhysicalDeviceProvider {

    public:
        virtual ~VulkanPhysicalDeviceProvider() = default;

        [[nodiscard]]
        virtual std::unique_ptr<VulkanPhysicalDeviceCandidate> provide() = 0;
    };

    class SimpleVulkanPhysicalDeviceProvider : public VulkanPhysicalDeviceProvider {
    private:
        std::unique_ptr<VulkanPhysicalDeviceCandidate> mCandidate;

    public:

        explicit SimpleVulkanPhysicalDeviceProvider(std::unique_ptr<VulkanPhysicalDeviceCandidate> &&candidate);

        ~SimpleVulkanPhysicalDeviceProvider() override;

        [[nodiscard]]
        std::unique_ptr<VulkanPhysicalDeviceCandidate> provide() override;
    };

    class DefaultVulkanPhysicalDeviceProvider : public VulkanPhysicalDeviceProvider {
    private:
        const VulkanInstance &mInstance;
        const VulkanSurface &mSurface;
        const std::vector<std::string> mDeviceExtensions;
        const VulkanPhysicalDeviceScoreCalculator mScoreCalculator;

    public:
        DefaultVulkanPhysicalDeviceProvider(const VulkanInstance &instance,
                                            const VulkanSurface &surface,
                                            const std::vector<std::string> &deviceExtensions,
                                            const VulkanPhysicalDeviceScoreConfig &scoreConfig = {});

        ~DefaultVulkanPhysicalDeviceProvider() override;

        [[nodiscard]]
        std::unique_ptr<VulkanPhysicalDeviceCandidate> provide() override;

    private:
        [[nodiscard]]
        std::optional<std::unique_ptr<VulkanPhysicalDeviceCandidate>> calcPhysicalDeviceCandidate(
                std::unique_ptr<VulkanPhysicalDevice> &&vulkanPhysicalDevice,
                const VulkanSurface &vulkanSurface,
                const std::vector<std::string> &requiredDeviceExtensions) const;
    };

} // engine
