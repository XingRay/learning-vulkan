//
// Created by leixing on 2024/12/23.
//

#include <map>

#include "engine/vulkan_wrapper/VulkanDevice.h"
#include "engine/Log.h"
#include "engine/VulkanUtil.h"

#include "engine/common/StringUtil.h"

namespace engine {

    VulkanDevice::VulkanDevice(const VulkanPhysicalDevice &physicalDevice,
                               const VulkanPhysicalDeviceSurfaceSupport &surfaceSupport,
                               const std::vector<std::string> &deviceExtensions,
                               const std::vector<std::string> &layers,
                               uint32_t sampleCount)
            : mPhysicalDevice(physicalDevice.getPhysicalDevice()) {

        mMsaaSamples = VulkanUtil::uint32ToSampleCountFlagBits(sampleCount);

        mGraphicQueueFamilyIndex = surfaceSupport.graphicQueueFamilyIndex;
        mPresentQueueFamilyIndex = surfaceSupport.presentQueueFamilyIndex;
        LOG_D("graphicFamilyIndex:%d, presentFamilyIndex:%d", mGraphicQueueFamilyIndex, mPresentQueueFamilyIndex);

        mQueueFamilyIndices.push_back(mGraphicQueueFamilyIndex);
        if (mGraphicQueueFamilyIndex != mPresentQueueFamilyIndex) {
            mQueueFamilyIndices.push_back(mPresentQueueFamilyIndex);
        }

        std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
        std::array<float, 1> queuePriorities = {1.0f};
        for (uint32_t queueFamilyIndex: mQueueFamilyIndices) {
            vk::DeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo
                    .setQueueFamilyIndex(queueFamilyIndex)
                    .setQueueCount(1)
                    .setQueuePriorities(queuePriorities);
            queueCreateInfos.push_back(queueCreateInfo);
        }

        vk::PhysicalDeviceFeatures supportedFeatures = mPhysicalDevice.getFeatures();
        vk::PhysicalDeviceFeatures deviceFeatures{};

        if (supportedFeatures.samplerAnisotropy) {
            deviceFeatures.setSamplerAnisotropy(vk::True);
        }
        if (supportedFeatures.sampleRateShading) {
            deviceFeatures.setSampleRateShading(vk::True);
        }

        std::vector<const char *> extensionNames = common::StringUtil::toStringPtrArray(deviceExtensions);
        std::vector<const char *> layerNames = common::StringUtil::toStringPtrArray(layers);

        vk::DeviceCreateInfo deviceCreateInfo;
        deviceCreateInfo
                .setQueueCreateInfos(queueCreateInfos)
                .setPEnabledFeatures(&deviceFeatures)
                .setPEnabledExtensionNames(extensionNames)
                .setPEnabledLayerNames(layerNames);

        mDevice = mPhysicalDevice.createDevice(deviceCreateInfo);

        mGraphicsQueue = mDevice.getQueue(mGraphicQueueFamilyIndex, 0);
        mPresentQueue = mDevice.getQueue(mPresentQueueFamilyIndex, 0);

        mCapabilities = surfaceSupport.capabilities;
        mFormats = surfaceSupport.formats;
        mPresentModes = surfaceSupport.presentModes;
    }

    VulkanDevice::~VulkanDevice() {
        LOG_D("VulkanDevice::~VulkanDevice");
        if (mDevice != nullptr) {
            mDevice.destroy();
        } else {
            LOG_W("mDevice is null");
        }
    }

    vk::PhysicalDevice VulkanDevice::getPhysicalDevice() const {
        return mPhysicalDevice;
    }

    vk::Device VulkanDevice::getDevice() const {
        return mDevice;
    }

    vk::SampleCountFlagBits VulkanDevice::getMsaaSamples() const {
        return mMsaaSamples;
    }

    uint32_t VulkanDevice::getGraphicQueueFamilyIndex() const {
        return mGraphicQueueFamilyIndex;
    }

    uint32_t VulkanDevice::getPresentQueueFamilyIndex() const {
        return mPresentQueueFamilyIndex;
    }

    const std::vector<uint32_t> &VulkanDevice::getQueueFamilyIndices() const {
        return mQueueFamilyIndices;
    }

    vk::Queue VulkanDevice::getGraphicsQueue() const {
        return mGraphicsQueue;
    }

    vk::Queue VulkanDevice::getPresentQueue() const {
        return mPresentQueue;
    }

    [[nodiscard]]
    vk::SurfaceCapabilitiesKHR VulkanDevice::getCapabilities() const {
        return mCapabilities;
    }

    [[nodiscard]]
    std::vector<vk::SurfaceFormatKHR> VulkanDevice::getFormats() const {
        return mFormats;
    }

    [[nodiscard]]
    std::vector<vk::PresentModeKHR> VulkanDevice::getPresentModes() const {
        return mPresentModes;
    }

    vk::ShaderModule VulkanDevice::createShaderModule(const std::vector<char> &code) const {
        vk::ShaderModuleCreateInfo createInfo;
        createInfo
                .setCodeSize(code.size())
                .setPCode(reinterpret_cast<const uint32_t *>(code.data()));

        return mDevice.createShaderModule(createInfo);
    }

} // engine