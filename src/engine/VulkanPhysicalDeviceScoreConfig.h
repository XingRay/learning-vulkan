//
// Created by leixing on 2025/1/11.
//

#pragma once

#include <stdint.h>

namespace engine {

    class VulkanPhysicalDeviceScoreConfig {
    private:
        // feature score
        int32_t geometryShaderScore;      // 几何着色器加分
        int32_t samplerAnisotropyScore;   // 各向异性过滤加分

        // device type score
        int32_t integratedGpuScore;       // 核显加分
        int32_t discreteGpuScore;         // 独显加分
        int32_t virtualGpuScore;          // 虚拟GPU加分
        int32_t cpuScore;                 // CPU加分
        int32_t otherDeviceTypeScore;     // 其他类型设备加分

    public:
        // 默认构造函数，设置默认分值
        VulkanPhysicalDeviceScoreConfig()
                : geometryShaderScore(10),
                  samplerAnisotropyScore(10),
                  integratedGpuScore(10),
                  discreteGpuScore(20),
                  virtualGpuScore(15),
                  cpuScore(5),
                  otherDeviceTypeScore(0) {}

        // 设置各项分值的方法（返回自身的引用，支持链式调用）
        VulkanPhysicalDeviceScoreConfig &setGeometryShaderScore(int score) {
            geometryShaderScore = score;
            return *this;
        }

        VulkanPhysicalDeviceScoreConfig &setSamplerAnisotropyScore(int score) {
            samplerAnisotropyScore = score;
            return *this;
        }

        VulkanPhysicalDeviceScoreConfig &setIntegratedGpuScore(int score) {
            integratedGpuScore = score;
            return *this;
        }

        VulkanPhysicalDeviceScoreConfig &setDiscreteGpuScore(int score) {
            discreteGpuScore = score;
            return *this;
        }

        VulkanPhysicalDeviceScoreConfig &setVirtualGpuScore(int score) {
            virtualGpuScore = score;
            return *this;
        }

        VulkanPhysicalDeviceScoreConfig &setCpuScore(int score) {
            cpuScore = score;
            return *this;
        }

        VulkanPhysicalDeviceScoreConfig &setOtherDeviceTypeScore(int score) {
            otherDeviceTypeScore = score;
            return *this;
        }

        // 获取各项分值的方法
        [[nodiscard]]
        int getGeometryShaderScore() const { return geometryShaderScore; }

        [[nodiscard]]
        int getSamplerAnisotropyScore() const { return samplerAnisotropyScore; }

        [[nodiscard]]
        int getIntegratedGpuScore() const { return integratedGpuScore; }

        [[nodiscard]]
        int getDiscreteGpuScore() const { return discreteGpuScore; }

        [[nodiscard]]
        int getVirtualGpuScore() const { return virtualGpuScore; }

        [[nodiscard]]
        int getCpuScore() const { return cpuScore; }

        [[nodiscard]]
        int getOtherDeviceTypeScore() const { return otherDeviceTypeScore; }
    };

} // engine
