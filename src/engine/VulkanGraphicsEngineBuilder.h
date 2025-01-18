//
// Created by leixing on 2025/1/10.
//

#pragma once

#include <vector>
#include <functional>

#include "vulkan/vulkan.hpp"

#include "engine/vulkan_wrapper/VulkanInstance.h"
#include "engine/VulkanPhysicalDeviceProvider.h"

#include "engine/VulkanGraphicsEngine.h"
#include "engine/VulkanShaderConfigure.h"

#include "engine/common/Selector.h"

namespace engine {

    class VulkanShaderConfigure;

    class VulkanVertex;

    class VulkanDescriptorSet;

    class VulkanGraphicsEngineBuilder {
        friend class VulkanShaderConfigure;

    private:
        std::unique_ptr<VulkanInstance> mInstance;
        std::unique_ptr<VulkanSurface> mSurface;
        std::vector<std::string> mDeviceExtensions;

        uint32_t mFrameCount = 2;

        std::unique_ptr<VulkanPhysicalDeviceProvider> mVulkanPhysicalDeviceProvider;
        std::unique_ptr<common::ValueSelector<uint32_t>> mMsaaSelector;

        // shader
        std::vector<char> mVertexShaderCode;
        std::vector<char> mFragmentShaderCode;
        std::vector<VulkanVertex> mVertices;
        std::vector<VulkanDescriptorSet> mDescriptorSets;
        std::vector<VulkanPushConstant> mPushConstants;

    public:
        VulkanGraphicsEngineBuilder(std::unique_ptr<VulkanInstance> instance);

        ~VulkanGraphicsEngineBuilder();

        VulkanGraphicsEngineBuilder &surface(const std::function<std::unique_ptr<VulkanSurface>(const VulkanInstance &)> &surfaceBuilder);

        VulkanGraphicsEngineBuilder &deviceExtensions(std::vector<std::string> &&deviceExtensions);

        VulkanGraphicsEngineBuilder &frameCount(uint32_t frameCount);

        VulkanGraphicsEngineBuilder &physicalDeviceAsDefault();

        VulkanGraphicsEngineBuilder &physicalDevice(std::unique_ptr<VulkanPhysicalDevice> vulkanPhysicalDevice);

        VulkanGraphicsEngineBuilder &physicalDeviceProvider(std::unique_ptr<VulkanPhysicalDeviceProvider> provider);

        VulkanGraphicsEngineBuilder &enableMsaa();

        VulkanGraphicsEngineBuilder &enableMsaa(uint32_t msaaSamples);

        VulkanGraphicsEngineBuilder &enableMsaaMax(uint32_t msaaSamplesMax);

        VulkanGraphicsEngineBuilder &enableMsaa(const std::function<uint32_t(const std::vector<uint32_t> &)> &selector);

        VulkanGraphicsEngineBuilder &shader(const std::function<void(VulkanShaderConfigure &)> &configure);

        VulkanGraphicsEngineBuilder &setVertexShaderCode(std::vector<char> &&code);

        VulkanGraphicsEngineBuilder &setFragmentShaderCode(std::vector<char> &&code);

        VulkanGraphicsEngineBuilder &setVertices(std::vector<VulkanVertex> &&vertices);

        VulkanGraphicsEngineBuilder &setUniformSets(std::vector<VulkanDescriptorSet> &&uniformSets);

        VulkanGraphicsEngineBuilder &setPushConstants(std::vector<VulkanPushConstant> &&pushConstants);

        std::unique_ptr<VulkanGraphicsEngine> build();
    };

} // engine
