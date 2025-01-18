//
// Created by leixing on 2025/1/10.
//


#include "engine/VulkanPhysicalDeviceCandidate.h"
#include "engine/Log.h"

namespace engine {

    VulkanPhysicalDeviceCandidate::VulkanPhysicalDeviceCandidate(std::unique_ptr<VulkanPhysicalDevice> &&physicalDevice)
            : mPhysicalDevice(std::move(physicalDevice)), mScore(0) {

    }

    VulkanPhysicalDeviceCandidate::~VulkanPhysicalDeviceCandidate() = default;

    VulkanPhysicalDeviceCandidate &VulkanPhysicalDeviceCandidate::plusScore(int32_t score) {
        mScore += score;
        return *this;
    }

    [[nodiscard]]
    std::unique_ptr<VulkanPhysicalDevice> &VulkanPhysicalDeviceCandidate::getPhysicalDevice() {
        return mPhysicalDevice;
    }

    // Getter and Setter for mScore
    [[nodiscard]]
    int32_t VulkanPhysicalDeviceCandidate::getScore() const {
        return mScore;
    }

    void VulkanPhysicalDeviceCandidate::setScore(int32_t score) {
        mScore = score;
    }

    // Getter for mSurfaceSupport
    [[nodiscard]]
    const std::optional<VulkanPhysicalDeviceSurfaceSupport> &VulkanPhysicalDeviceCandidate::getSurfaceSupport() const {
        return mSurfaceSupport;
    }

    // Setter for mSurfaceSupport (拷贝语义)
    void VulkanPhysicalDeviceCandidate::setSurfaceSupport(const VulkanPhysicalDeviceSurfaceSupport &support) {
        mSurfaceSupport = support;
    }

    // Setter for mSurfaceSupport (移动语义)
    void VulkanPhysicalDeviceCandidate::setSurfaceSupport(VulkanPhysicalDeviceSurfaceSupport &&support) {
        mSurfaceSupport = std::move(support);
    }

    // 重置 mSurfaceSupport 为空
    void VulkanPhysicalDeviceCandidate::resetSurfaceSupport() {
        mSurfaceSupport.reset();
    }

    // 检查 mSurfaceSupport 是否有值
    [[nodiscard]]
    bool VulkanPhysicalDeviceCandidate::hasSurfaceSupport() const {
        return mSurfaceSupport.has_value();
    }
}