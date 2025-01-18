//
// Created by leixing on 2025/1/9.
//

#include "engine/VulkanPhysicalDeviceProvider.h"
#include "engine/Log.h"
#include "engine/VulkanPhysicalDeviceCandidate.h"
#include "engine/VulkanUtil.h"
#include "engine/common/StringUtil.h"

namespace engine {

    SimpleVulkanPhysicalDeviceProvider::SimpleVulkanPhysicalDeviceProvider(std::unique_ptr<VulkanPhysicalDeviceCandidate> &&candidate)
            : mCandidate(std::move(candidate)) {

    }

    SimpleVulkanPhysicalDeviceProvider::~SimpleVulkanPhysicalDeviceProvider() = default;

    std::unique_ptr<VulkanPhysicalDeviceCandidate> SimpleVulkanPhysicalDeviceProvider::provide() {
        return std::move(mCandidate);
    }


    DefaultVulkanPhysicalDeviceProvider::DefaultVulkanPhysicalDeviceProvider(const VulkanInstance &instance,
                                                                             const VulkanSurface &surface,
                                                                             const std::vector<std::string> &deviceExtensions,
                                                                             const VulkanPhysicalDeviceScoreConfig &scoreConfig)
            : mInstance(instance), mSurface(surface), mDeviceExtensions(deviceExtensions), mScoreCalculator(scoreConfig) {

    }

    DefaultVulkanPhysicalDeviceProvider::~DefaultVulkanPhysicalDeviceProvider() = default;

    std::unique_ptr<VulkanPhysicalDeviceCandidate> DefaultVulkanPhysicalDeviceProvider::provide() {
        std::vector<std::unique_ptr<VulkanPhysicalDevice>> physicalDevices = mInstance.listPhysicalDevices();
        if (physicalDevices.empty()) {
            LOG_D("No physical devices found!");
            throw std::runtime_error("No physical devices found!");
        }

        std::vector<std::unique_ptr<VulkanPhysicalDeviceCandidate>> candidates;
        for (auto &physicalDevice: physicalDevices) {
            auto result = calcPhysicalDeviceCandidate(std::move(physicalDevice), mSurface, mDeviceExtensions);
            if (result.has_value()) {
                candidates.push_back(std::move(result.value()));
            }
        }
        if (candidates.empty()) {
            LOG_D("No suitable candidates found!");
            throw std::runtime_error("No suitable candidates found!");
        }

        auto best = std::max_element(candidates.begin(), candidates.end(),
                                     [](const std::unique_ptr<VulkanPhysicalDeviceCandidate> &a, std::unique_ptr<VulkanPhysicalDeviceCandidate> &b) -> bool {
                                         return a->getScore() < b->getScore();
                                     });

        return std::move(*best);
    }

    std::optional<std::unique_ptr<VulkanPhysicalDeviceCandidate>> DefaultVulkanPhysicalDeviceProvider::calcPhysicalDeviceCandidate(
            std::unique_ptr<VulkanPhysicalDevice> &&vulkanPhysicalDevice,
            const VulkanSurface &vulkanSurface,
            const std::vector<std::string> &requiredDeviceExtensions) const {

        // 检查扩展支持
        if (!vulkanPhysicalDevice->isSupportExtensions(requiredDeviceExtensions)) {
            LOG_D("Physical device does not support all required extensions.");
            return std::nullopt; // 返回空值，表示无效候选项
        }

        // 检查表面支持
        auto surfaceSupport = vulkanPhysicalDevice->querySurfaceSupport(vulkanSurface);
        if (!surfaceSupport.has_value()) {
            LOG_W("Device queue family indices are not complete.");
            return std::nullopt; // 返回空值，表示无效候选项
        }

        // 前两项检查通过，创建候选对象
        auto candidate = std::make_unique<VulkanPhysicalDeviceCandidate>(std::move(vulkanPhysicalDevice));
        candidate->setSurfaceSupport(*surfaceSupport);

        // 计算分数
        const auto &physicalDevice = candidate->getPhysicalDevice()->getPhysicalDevice();
        vk::PhysicalDeviceProperties deviceProperties = physicalDevice.getProperties();
        vk::PhysicalDeviceFeatures deviceFeatures = physicalDevice.getFeatures();
        candidate->plusScore(mScoreCalculator.calculateScore(deviceProperties, deviceFeatures));

        // 返回有效的候选项
        return candidate;
    }
} // engine