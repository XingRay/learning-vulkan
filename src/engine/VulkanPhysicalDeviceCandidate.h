//
// Created by leixing on 2025/1/9.
//

#pragma once

#include <memory>

#include "vulkan/vulkan.hpp"

#include "engine/vulkan_wrapper/VulkanPhysicalDevice.h"
#include "engine/SwapChainSupportDetail.h"
#include "engine/VulkanPhysicalDeviceSurfaceSupport.h"

namespace engine {

    class VulkanPhysicalDeviceCandidate {
    private:
        std::unique_ptr<VulkanPhysicalDevice> mPhysicalDevice;

        int32_t mScore;

        std::optional<VulkanPhysicalDeviceSurfaceSupport> mSurfaceSupport;

    public:
        explicit VulkanPhysicalDeviceCandidate(std::unique_ptr<VulkanPhysicalDevice>&& physicalDevice);

        ~VulkanPhysicalDeviceCandidate();

        VulkanPhysicalDeviceCandidate &plusScore(int32_t score);


        [[nodiscard]]
        std::unique_ptr<VulkanPhysicalDevice> &getPhysicalDevice();

        // Getter and Setter for mScore
        [[nodiscard]]
        int32_t getScore() const;

        void setScore(int32_t score);


        // Getter for mSurfaceSupport
        [[nodiscard]]
        const std::optional<VulkanPhysicalDeviceSurfaceSupport> &getSurfaceSupport() const;

        // Setter for mSurfaceSupport (拷贝语义)
        void setSurfaceSupport(const VulkanPhysicalDeviceSurfaceSupport &support);

        // Setter for mSurfaceSupport (移动语义)
        void setSurfaceSupport(VulkanPhysicalDeviceSurfaceSupport &&support);

        // 重置 mSurfaceSupport 为空
        void resetSurfaceSupport();

        // 检查 mSurfaceSupport 是否有值
        [[nodiscard]]
        bool hasSurfaceSupport() const;

    };

} // engine
