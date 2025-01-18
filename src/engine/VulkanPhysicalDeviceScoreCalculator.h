//
// Created by leixing on 2025/1/11.
//

#pragma once

#include "vulkan/vulkan.hpp"
#include "engine/VulkanPhysicalDeviceScoreConfig.h"
#include <stdint.h>

namespace engine {

    class VulkanPhysicalDeviceScoreCalculator {
    private:
        const VulkanPhysicalDeviceScoreConfig &mScoreConfig;

    public:
        VulkanPhysicalDeviceScoreCalculator(const VulkanPhysicalDeviceScoreConfig &scoreConfig) : mScoreConfig(scoreConfig) {}

        [[nodiscard]]
        int32_t calculateScore(const vk::PhysicalDeviceProperties &properties, const vk::PhysicalDeviceFeatures &features) const {
            int32_t score = 0;

            if (features.geometryShader) {
                score += mScoreConfig.getGeometryShaderScore();
            }

            if (features.samplerAnisotropy) {
                score += mScoreConfig.getSamplerAnisotropyScore();
            }

            switch (properties.deviceType) {
                case vk::PhysicalDeviceType::eIntegratedGpu:
                    score += mScoreConfig.getIntegratedGpuScore();
                    break;
                case vk::PhysicalDeviceType::eDiscreteGpu:
                    score += mScoreConfig.getDiscreteGpuScore();
                    break;
                case vk::PhysicalDeviceType::eVirtualGpu:
                    score += mScoreConfig.getVirtualGpuScore();
                    break;
                case vk::PhysicalDeviceType::eCpu:
                    score += mScoreConfig.getCpuScore();
                    break;
                case vk::PhysicalDeviceType::eOther:
                default:
                    score += mScoreConfig.getOtherDeviceTypeScore();
                    break;
            }

            return score;
        }
    };

} // engine
