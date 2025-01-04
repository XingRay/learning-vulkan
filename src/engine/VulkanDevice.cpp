//
// Created by leixing on 2024/12/23.
//

#include "VulkanDevice.h"
#include "Log.h"
#include <set>
#include <map>

namespace engine {

    VulkanDevice::VulkanDevice(const vk::Instance &instance,
                               const vk::SurfaceKHR &surface,
                               const std::vector<const char *> &layers,
                               const std::vector<const char *> &deviceExtensions) {
        LOG_D("pickPhysicalDevice");
        auto devices = instance.enumeratePhysicalDevices();
        for (const auto &device: devices) {
            vk::PhysicalDeviceProperties properties = device.getProperties();
            LOG_D("\tdeviceName: %s\tdeviceType: %s", properties.deviceName.data(), to_string(properties.deviceType).data());
        }

        // 计算每张卡的分数, 取最高分
        std::unique_ptr<PhysicalDeviceCandidate> candidate = nullptr;
        std::multimap<int32_t, std::unique_ptr<PhysicalDeviceCandidate>> candidates;
        for (const auto &device: devices) {
//            printPhysicalDeviceInfo(device, surface);
            candidates.insert(calcDeviceSuitable(device, deviceExtensions, surface));
        }
        if (candidates.rbegin()->first > 0) {
            candidate = std::move(candidates.rbegin()->second);
        } else {
            LOG_D("failed to find GPUs with vulkan support !");
            throw std::runtime_error("failed to find GPUs with vulkan support !");
        }

        mPhysicalDevice = candidate->physicalDevice;
        mMsaaSamples = getMaxUsableSampleCount(mPhysicalDevice);
        mGraphicQueueFamilyIndex = candidate->graphicQueueFamilyIndex.value();
        mPresentQueueFamilyIndex = candidate->presentQueueFamilyIndex.value();
        LOG_D("graphicFamilyIndex:%d, presentFamilyIndex:%d", mGraphicQueueFamilyIndex, mPresentQueueFamilyIndex);

        mQueueFamilyIndices.push_back(mGraphicQueueFamilyIndex);
        if (mGraphicQueueFamilyIndex != mPresentQueueFamilyIndex) {
            mQueueFamilyIndices.push_back(mPresentQueueFamilyIndex);
        }

        std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
        std::array<float, 1> queuePriorities = {1.0f};
        for (uint32_t queueFamilyIndex: mQueueFamilyIndices) {
            vk::DeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.setQueueFamilyIndex(queueFamilyIndex)
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

        vk::DeviceCreateInfo deviceCreateInfo;
        deviceCreateInfo
                .setQueueCreateInfos(queueCreateInfos)
                .setPEnabledFeatures(&deviceFeatures)
                .setPEnabledExtensionNames(deviceExtensions)
                .setPEnabledLayerNames(layers);

        mDevice = mPhysicalDevice.createDevice(deviceCreateInfo);

        mGraphicsQueue = mDevice.getQueue(mGraphicQueueFamilyIndex, 0);
        mPresentQueue = mDevice.getQueue(mPresentQueueFamilyIndex, 0);

        mCapabilities = mPhysicalDevice.getSurfaceCapabilitiesKHR(surface);
        mFormats = mPhysicalDevice.getSurfaceFormatsKHR(surface);
        mPresentModes = mPhysicalDevice.getSurfacePresentModesKHR(surface);
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

    std::pair<int32_t, std::unique_ptr<PhysicalDeviceCandidate>> VulkanDevice::calcDeviceSuitable(
            const vk::PhysicalDevice &device,
            const std::vector<const char *> &requiredDeviceExtensions,
            const vk::SurfaceKHR &surface) {
        int32_t score = 0;

        vk::PhysicalDeviceProperties deviceProperties = device.getProperties();
        vk::PhysicalDeviceFeatures deviceFeatures = device.getFeatures();

        QueueFamilyIndices indices = findQueueFamilies(device, surface);
        if (!indices.isComplete()) {
            LOG_D("device QueueFamilyIndices is not complete !");
            return std::make_pair(score, nullptr);
        }

        if (!isDeviceSupportedRequiredDeviceExtensions(device, requiredDeviceExtensions)) {
            LOG_D("isDeviceSupportedRequiredDeviceExtensions: false");
            return std::make_pair(score, nullptr);
        }

        // 验证扩展可用后才尝试查询交换链支持
        SwapChainSupportDetail swapChainSupportedDetail = querySwapChainSupported(device, surface);
        if (swapChainSupportedDetail.formats.empty() || swapChainSupportedDetail.presentModes.empty()) {
            LOG_D("swapChainSupportedDetail: formats or presentModes is empty");
            return std::make_pair(score, nullptr);
        }

        if (!deviceFeatures.geometryShader || !deviceFeatures.samplerAnisotropy) {
            return std::make_pair(score, nullptr);
        }

        switch (deviceProperties.deviceType) {
            case vk::PhysicalDeviceType::eIntegratedGpu:
                // 核显
                score += 10;
                break;

            case vk::PhysicalDeviceType::eDiscreteGpu:
                // 独显
                score += 20;
                break;

            case vk::PhysicalDeviceType::eVirtualGpu:
                // 虚拟GPU
                score += 15; // 虚拟GPU在性能上可能低于独显，但高于CPU
                break;

            case vk::PhysicalDeviceType::eCpu:
                // CPU
                score += 5;  // CPU的性能通常较低，分数也低
                break;

            case vk::PhysicalDeviceType::eOther:
            default:
                // 其他类型的设备
                score += 0;  // 对于未知类型或其他类型，默认不加分
                break;
        }

        std::unique_ptr<PhysicalDeviceCandidate> candidate = std::make_unique<PhysicalDeviceCandidate>(device, indices.graphicQueueFamilyIndex, indices.presentQueueFamilyIndex,
                                                                                                       swapChainSupportedDetail);
        return std::make_pair(score, std::move(candidate));
    }

    QueueFamilyIndices VulkanDevice::findQueueFamilies(const vk::PhysicalDevice &device, const vk::SurfaceKHR &surface) {
        QueueFamilyIndices indices;
        auto queueFamilyProperties = device.getQueueFamilyProperties();

        for (int i = 0; i < queueFamilyProperties.size(); i++) {
            const auto &queueFamilyProperty = queueFamilyProperties[i];
            const vk::QueueFlags &queueFlags = queueFamilyProperty.queueFlags;

            if (queueFlags & vk::QueueFlagBits::eGraphics) {
                LOG_D("graphicQueueFamily found, index:%d", i);
                indices.graphicQueueFamilyIndex = i;
            }

            if (device.getSurfaceSupportKHR(i, surface)) {
                LOG_D("presentQueueFamily found, index:%d", i);
                indices.presentQueueFamilyIndex = i;
            }

            if (indices.isComplete()) {
                break;
            }
        }

        return indices;
    }

    vk::SampleCountFlagBits VulkanDevice::getMaxUsableSampleCount(const vk::PhysicalDevice &device) {
        vk::PhysicalDeviceProperties properties = device.getProperties();
        vk::PhysicalDeviceLimits &limits = properties.limits;

        vk::SampleCountFlags counts = limits.framebufferColorSampleCounts & limits.framebufferDepthSampleCounts;
        if (counts & vk::SampleCountFlagBits::e64) {
            return vk::SampleCountFlagBits::e64;
        } else if (counts & vk::SampleCountFlagBits::e32) {
            return vk::SampleCountFlagBits::e32;
        } else if (counts & vk::SampleCountFlagBits::e16) {
            return vk::SampleCountFlagBits::e16;
        } else if (counts & vk::SampleCountFlagBits::e8) {
            return vk::SampleCountFlagBits::e8;
        } else if (counts & vk::SampleCountFlagBits::e4) {
            return vk::SampleCountFlagBits::e4;
        } else if (counts & vk::SampleCountFlagBits::e2) {
            return vk::SampleCountFlagBits::e2;
        } else {
            return vk::SampleCountFlagBits::e1;
        }
    }

    bool VulkanDevice::isDeviceSupportedRequiredDeviceExtensions(const vk::PhysicalDevice &device,
                                                                 const std::vector<const char *> &requiredDeviceExtensions) {
        LOG_D("requiredExtensions");
        for (const auto &extension: requiredDeviceExtensions) {
            LOG_D("\t:%s", extension);
        }

        std::set<std::string> requiredExtensionSet(requiredDeviceExtensions.begin(), requiredDeviceExtensions.end());

        std::vector<vk::ExtensionProperties> properties = device.enumerateDeviceExtensionProperties();
//        LOG_D("device.enumerateDeviceExtensionProperties():");
        for (const auto &property: properties) {
//            LOG_D("\tproperty.extensionName: %s", property.extensionName.data());
            requiredExtensionSet.erase(property.extensionName);
        }
        return requiredExtensionSet.empty();
    }

    SwapChainSupportDetail VulkanDevice::querySwapChainSupported(const vk::PhysicalDevice &device, const vk::SurfaceKHR &surface) {
        SwapChainSupportDetail detail;

        detail.capabilities = device.getSurfaceCapabilitiesKHR(surface);
        detail.formats = device.getSurfaceFormatsKHR(surface);
        detail.presentModes = device.getSurfacePresentModesKHR(surface);

        return detail;
    }

    void VulkanDevice::printPhysicalDeviceInfo(const vk::PhysicalDevice &physicalDevice, const vk::SurfaceKHR &surface) {
        // 获取物理设备的属性
        vk::PhysicalDeviceProperties properties = physicalDevice.getProperties();

        LOG_D("Physical Device Properties:");
        LOG_D("  Device Name: %s", properties.deviceName.data());
        LOG_D("  API Version: %u.%u.%u",
              VK_VERSION_MAJOR(properties.apiVersion),
              VK_VERSION_MINOR(properties.apiVersion),
              VK_VERSION_PATCH(properties.apiVersion));
        LOG_D("  Driver Version: %u", properties.driverVersion);
        LOG_D("  Vendor ID: 0x%04X", properties.vendorID);
        LOG_D("  Device ID: 0x%04X", properties.deviceID);
        LOG_D("  Device Type: %s", vk::to_string(properties.deviceType).c_str());
        LOG_D("  Pipeline Cache UUID: ");
        for (uint8_t byte: properties.pipelineCacheUUID) {
            LOG_D("    %02X", byte);
        }

        // 打印 PhysicalDeviceLimits
        const vk::PhysicalDeviceLimits &limits = properties.limits;

        LOG_D("Physical Device Limits:");
        LOG_D("  Max Image Dimension 1D: %u", limits.maxImageDimension1D);
        LOG_D("  Max Image Dimension 2D: %u", limits.maxImageDimension2D);
        LOG_D("  Max Image Dimension 3D: %u", limits.maxImageDimension3D);
        LOG_D("  Max Image Dimension Cube: %u", limits.maxImageDimensionCube);
        LOG_D("  Max Image Array Layers: %u", limits.maxImageArrayLayers);
        LOG_D("  Max Texel Buffer Elements: %u", limits.maxTexelBufferElements);
        LOG_D("  Max Uniform Buffer Range: %u", limits.maxUniformBufferRange);
        LOG_D("  Max Storage Buffer Range: %u", limits.maxStorageBufferRange);
        LOG_D("  Max Push Constants Size: %u", limits.maxPushConstantsSize);
        LOG_D("  Max Memory Allocation Count: %u", limits.maxMemoryAllocationCount);
        LOG_D("  Max Sampler Allocation Count: %u", limits.maxSamplerAllocationCount);
        LOG_D("  Buffer Image Granularity: %zu", limits.bufferImageGranularity);
        LOG_D("  Sparse Address Space Size: %zu", limits.sparseAddressSpaceSize);
        LOG_D("  Max Bound Descriptor Sets: %u", limits.maxBoundDescriptorSets);
        LOG_D("  Max Per Stage Descriptor Samplers: %u", limits.maxPerStageDescriptorSamplers);
        LOG_D("  Max Per Stage Descriptor Uniform Buffers: %u", limits.maxPerStageDescriptorUniformBuffers);
        LOG_D("  Max Per Stage Descriptor Storage Buffers: %u", limits.maxPerStageDescriptorStorageBuffers);
        LOG_D("  Max Per Stage Descriptor Sampled Images: %u", limits.maxPerStageDescriptorSampledImages);
        LOG_D("  Max Per Stage Descriptor Storage Images: %u", limits.maxPerStageDescriptorStorageImages);
        LOG_D("  Max Per Stage Descriptor Input Attachments: %u", limits.maxPerStageDescriptorInputAttachments);
        LOG_D("  Max Per Stage Resources: %u", limits.maxPerStageResources);
        LOG_D("  Max Descriptor Set Samplers: %u", limits.maxDescriptorSetSamplers);
        LOG_D("  Max Descriptor Set Uniform Buffers: %u", limits.maxDescriptorSetUniformBuffers);
        LOG_D("  Max Descriptor Set Uniform Buffers Dynamic: %u", limits.maxDescriptorSetUniformBuffersDynamic);
        LOG_D("  Max Descriptor Set Storage Buffers: %u", limits.maxDescriptorSetStorageBuffers);
        LOG_D("  Max Descriptor Set Storage Buffers Dynamic: %u", limits.maxDescriptorSetStorageBuffersDynamic);
        LOG_D("  Max Descriptor Set Sampled Images: %u", limits.maxDescriptorSetSampledImages);
        LOG_D("  Max Descriptor Set Storage Images: %u", limits.maxDescriptorSetStorageImages);
        LOG_D("  Max Descriptor Set Input Attachments: %u", limits.maxDescriptorSetInputAttachments);
        LOG_D("  Max Vertex Input Attributes: %u", limits.maxVertexInputAttributes);
        LOG_D("  Max Vertex Input Bindings: %u", limits.maxVertexInputBindings);
        LOG_D("  Max Vertex Input Attribute Offset: %u", limits.maxVertexInputAttributeOffset);
        LOG_D("  Max Vertex Input Binding Stride: %u", limits.maxVertexInputBindingStride);
        LOG_D("  Max Vertex Output Components: %u", limits.maxVertexOutputComponents);
        LOG_D("  Max Tessellation Generation Level: %u", limits.maxTessellationGenerationLevel);
        LOG_D("  Max Tessellation Patch Size: %u", limits.maxTessellationPatchSize);
        LOG_D("  Max Tessellation Control Per Vertex Input Components: %u", limits.maxTessellationControlPerVertexInputComponents);
        LOG_D("  Max Tessellation Control Per Vertex Output Components: %u", limits.maxTessellationControlPerVertexOutputComponents);
        LOG_D("  Max Tessellation Control Per Patch Output Components: %u", limits.maxTessellationControlPerPatchOutputComponents);
        LOG_D("  Max Tessellation Control Total Output Components: %u", limits.maxTessellationControlTotalOutputComponents);
        LOG_D("  Max Tessellation Evaluation Input Components: %u", limits.maxTessellationEvaluationInputComponents);
        LOG_D("  Max Tessellation Evaluation Output Components: %u", limits.maxTessellationEvaluationOutputComponents);
        LOG_D("  Max Geometry Shader Invocations: %u", limits.maxGeometryShaderInvocations);
        LOG_D("  Max Geometry Input Components: %u", limits.maxGeometryInputComponents);
        LOG_D("  Max Geometry Output Components: %u", limits.maxGeometryOutputComponents);
        LOG_D("  Max Geometry Output Vertices: %u", limits.maxGeometryOutputVertices);
        LOG_D("  Max Geometry Total Output Components: %u", limits.maxGeometryTotalOutputComponents);
        LOG_D("  Max Fragment Input Components: %u", limits.maxFragmentInputComponents);
        LOG_D("  Max Fragment Output Attachments: %u", limits.maxFragmentOutputAttachments);
        LOG_D("  Max Fragment Dual Src Attachments: %u", limits.maxFragmentDualSrcAttachments);
        LOG_D("  Max Fragment Combined Output Resources: %u", limits.maxFragmentCombinedOutputResources);
        LOG_D("  Max Compute Shared Memory Size: %u", limits.maxComputeSharedMemorySize);
        LOG_D("  Max Compute Work Group Count (x, y, z): %u, %u, %u", limits.maxComputeWorkGroupCount[0], limits.maxComputeWorkGroupCount[1], limits.maxComputeWorkGroupCount[2]);
        LOG_D("  Max Compute Work Group Invocations: %u", limits.maxComputeWorkGroupInvocations);
        LOG_D("  Max Compute Work Group Size (x, y, z): %u, %u, %u", limits.maxComputeWorkGroupSize[0], limits.maxComputeWorkGroupSize[1], limits.maxComputeWorkGroupSize[2]);
        LOG_D("  Sub Pixel Precision Bits: %u", limits.subPixelPrecisionBits);
        LOG_D("  Sub Texel Precision Bits: %u", limits.subTexelPrecisionBits);
        LOG_D("  Mipmap Precision Bits: %u", limits.mipmapPrecisionBits);
        LOG_D("  Max Draw Indexed Index Value: %u", limits.maxDrawIndexedIndexValue);
        LOG_D("  Max Draw Indirect Count: %u", limits.maxDrawIndirectCount);
        LOG_D("  Max Sampler Lod Bias: %f", limits.maxSamplerLodBias);
        LOG_D("  Max Sampler Anisotropy: %f", limits.maxSamplerAnisotropy);
        LOG_D("  Max Viewports: %u", limits.maxViewports);
        LOG_D("  Max Viewport Dimensions (x, y): %u, %u", limits.maxViewportDimensions[0], limits.maxViewportDimensions[1]);
        LOG_D("  Viewport Bounds Range (min, max): %f, %f", limits.viewportBoundsRange[0], limits.viewportBoundsRange[1]);
        LOG_D("  Viewport Sub Pixel Bits: %u", limits.viewportSubPixelBits);
        LOG_D("  Min Memory Map Alignment: %zu", limits.minMemoryMapAlignment);
        LOG_D("  Min Texel Buffer Offset Alignment: %zu", limits.minTexelBufferOffsetAlignment);
        LOG_D("  Min Uniform Buffer Offset Alignment: %zu", limits.minUniformBufferOffsetAlignment);
        LOG_D("  Min Storage Buffer Offset Alignment: %zu", limits.minStorageBufferOffsetAlignment);
        LOG_D("  Min Texel Offset: %d", limits.minTexelOffset);
        LOG_D("  Max Texel Offset: %u", limits.maxTexelOffset);
        LOG_D("  Min Texel Gather Offset: %d", limits.minTexelGatherOffset);
        LOG_D("  Max Texel Gather Offset: %u", limits.maxTexelGatherOffset);
        LOG_D("  Min Interpolation Offset: %f", limits.minInterpolationOffset);
        LOG_D("  Max Interpolation Offset: %f", limits.maxInterpolationOffset);
        LOG_D("  Sub Pixel Interpolation Offset Bits: %u", limits.subPixelInterpolationOffsetBits);
        LOG_D("  Max Framebuffer Width: %u", limits.maxFramebufferWidth);
        LOG_D("  Max Framebuffer Height: %u", limits.maxFramebufferHeight);
        LOG_D("  Max Framebuffer Layers: %u", limits.maxFramebufferLayers);
        LOG_D("  Framebuffer Color Sample Counts: %s", sampleCountFlagsToString(limits.framebufferColorSampleCounts).c_str());
        LOG_D("  Framebuffer Depth Sample Counts: %s", sampleCountFlagsToString(limits.framebufferDepthSampleCounts).c_str());
        LOG_D("  Framebuffer Stencil Sample Counts: %s", sampleCountFlagsToString(limits.framebufferStencilSampleCounts).c_str());
        LOG_D("  Framebuffer No Attachments Sample Counts: %s", sampleCountFlagsToString(limits.framebufferNoAttachmentsSampleCounts).c_str());
        LOG_D("  Max Color Attachments: %u", limits.maxColorAttachments);
        LOG_D("  Sampled Image Color Sample Counts: %s", sampleCountFlagsToString(limits.sampledImageColorSampleCounts).c_str());
        LOG_D("  Sampled Image Integer Sample Counts: %s", sampleCountFlagsToString(limits.sampledImageIntegerSampleCounts).c_str());
        LOG_D("  Sampled Image Depth Sample Counts: %s", sampleCountFlagsToString(limits.sampledImageDepthSampleCounts).c_str());
        LOG_D("  Sampled Image Stencil Sample Counts: %s", sampleCountFlagsToString(limits.sampledImageStencilSampleCounts).c_str());
        LOG_D("  Storage Image Sample Counts: %s", sampleCountFlagsToString(limits.storageImageSampleCounts).c_str());
        LOG_D("  Max Sample Mask Words: %u", limits.maxSampleMaskWords);
        LOG_D("  Timestamp Compute and Graphics: %s", limits.timestampComputeAndGraphics ? "Supported" : "Not Supported");
        LOG_D("  Timestamp Period: %f", limits.timestampPeriod);
        LOG_D("  Max Clip Distances: %u", limits.maxClipDistances);
        LOG_D("  Max Cull Distances: %u", limits.maxCullDistances);
        LOG_D("  Max Combined Clip and Cull Distances: %u", limits.maxCombinedClipAndCullDistances);
        LOG_D("  Discrete Queue Priorities: %u", limits.discreteQueuePriorities);
        LOG_D("  Point Size Range: %f, %f", limits.pointSizeRange[0], limits.pointSizeRange[1]);
        LOG_D("  Line Width Range: %f, %f", limits.lineWidthRange[0], limits.lineWidthRange[1]);
        LOG_D("  Point Size Granularity: %f", limits.pointSizeGranularity);
        LOG_D("  Line Width Granularity: %f", limits.lineWidthGranularity);
        LOG_D("  Strict Lines: %s", limits.strictLines ? "Supported" : "Not Supported");
        LOG_D("  Standard Sample Locations: %s", limits.standardSampleLocations ? "Supported" : "Not Supported");
        LOG_D("  Optimal Buffer Copy Offset Alignment: %zu", limits.optimalBufferCopyOffsetAlignment);
        LOG_D("  Optimal Buffer Copy Row Pitch Alignment: %zu", limits.optimalBufferCopyRowPitchAlignment);
        LOG_D("  Non-Coherent Atom Size: %zu", limits.nonCoherentAtomSize);

        // 打印 PhysicalDeviceSparseProperties
        const vk::PhysicalDeviceSparseProperties &sparseProperties = properties.sparseProperties;
        LOG_D("Physical Device Sparse Properties:");
        LOG_D("  Residency Standard 2D Block Shape: %s", sparseProperties.residencyStandard2DBlockShape ? "Supported" : "Not Supported");
        LOG_D("  Residency Standard 2D Multisample Block Shape: %s", sparseProperties.residencyStandard2DMultisampleBlockShape ? "Supported" : "Not Supported");
        LOG_D("  Residency Standard 3D Block Shape: %s", sparseProperties.residencyStandard3DBlockShape ? "Supported" : "Not Supported");
        LOG_D("  Residency Aligned Mip Size: %s", sparseProperties.residencyAlignedMipSize ? "Supported" : "Not Supported");
        LOG_D("  Residency Non-Resident Strict: %s", sparseProperties.residencyNonResidentStrict ? "Supported" : "Not Supported");

        // 获取设备支持的特性
        auto deviceFeatures = physicalDevice.getFeatures();
        LOG_D("PhysicalDeviceFeatures:");
        LOG_D("robustBufferAccess: %s", deviceFeatures.robustBufferAccess ? "Enabled" : "Disabled");
        LOG_D("fullDrawIndexUint32: %s", deviceFeatures.fullDrawIndexUint32 ? "Enabled" : "Disabled");
        LOG_D("imageCubeArray: %s", deviceFeatures.imageCubeArray ? "Enabled" : "Disabled");
        LOG_D("independentBlend: %s", deviceFeatures.independentBlend ? "Enabled" : "Disabled");
        LOG_D("geometryShader: %s", deviceFeatures.geometryShader ? "Enabled" : "Disabled");
        LOG_D("tessellationShader: %s", deviceFeatures.tessellationShader ? "Enabled" : "Disabled");
        LOG_D("sampleRateShading: %s", deviceFeatures.sampleRateShading ? "Enabled" : "Disabled");
        LOG_D("dualSrcBlend: %s", deviceFeatures.dualSrcBlend ? "Enabled" : "Disabled");
        LOG_D("logicOp: %s", deviceFeatures.logicOp ? "Enabled" : "Disabled");
        LOG_D("multiDrawIndirect: %s", deviceFeatures.multiDrawIndirect ? "Enabled" : "Disabled");
        LOG_D("drawIndirectFirstInstance: %s", deviceFeatures.drawIndirectFirstInstance ? "Enabled" : "Disabled");
        LOG_D("depthClamp: %s", deviceFeatures.depthClamp ? "Enabled" : "Disabled");
        LOG_D("depthBiasClamp: %s", deviceFeatures.depthBiasClamp ? "Enabled" : "Disabled");
        LOG_D("fillModeNonSolid: %s", deviceFeatures.fillModeNonSolid ? "Enabled" : "Disabled");
        LOG_D("depthBounds: %s", deviceFeatures.depthBounds ? "Enabled" : "Disabled");
        LOG_D("wideLines: %s", deviceFeatures.wideLines ? "Enabled" : "Disabled");
        LOG_D("largePoints: %s", deviceFeatures.largePoints ? "Enabled" : "Disabled");
        LOG_D("alphaToOne: %s", deviceFeatures.alphaToOne ? "Enabled" : "Disabled");
        LOG_D("multiViewport: %s", deviceFeatures.multiViewport ? "Enabled" : "Disabled");
        LOG_D("samplerAnisotropy: %s", deviceFeatures.samplerAnisotropy ? "Enabled" : "Disabled");
        LOG_D("textureCompressionETC2: %s", deviceFeatures.textureCompressionETC2 ? "Enabled" : "Disabled");
        LOG_D("textureCompressionASTC_LDR: %s", deviceFeatures.textureCompressionASTC_LDR ? "Enabled" : "Disabled");
        LOG_D("textureCompressionBC: %s", deviceFeatures.textureCompressionBC ? "Enabled" : "Disabled");
        LOG_D("occlusionQueryPrecise: %s", deviceFeatures.occlusionQueryPrecise ? "Enabled" : "Disabled");
        LOG_D("pipelineStatisticsQuery: %s", deviceFeatures.pipelineStatisticsQuery ? "Enabled" : "Disabled");
        LOG_D("vertexPipelineStoresAndAtomics: %s", deviceFeatures.vertexPipelineStoresAndAtomics ? "Enabled" : "Disabled");
        LOG_D("fragmentStoresAndAtomics: %s", deviceFeatures.fragmentStoresAndAtomics ? "Enabled" : "Disabled");
        LOG_D("shaderTessellationAndGeometryPointSize: %s", deviceFeatures.shaderTessellationAndGeometryPointSize ? "Enabled" : "Disabled");
        LOG_D("shaderImageGatherExtended: %s", deviceFeatures.shaderImageGatherExtended ? "Enabled" : "Disabled");
        LOG_D("shaderStorageImageExtendedFormats: %s", deviceFeatures.shaderStorageImageExtendedFormats ? "Enabled" : "Disabled");
        LOG_D("shaderStorageImageMultisample: %s", deviceFeatures.shaderStorageImageMultisample ? "Enabled" : "Disabled");
        LOG_D("shaderStorageImageReadWithoutFormat: %s", deviceFeatures.shaderStorageImageReadWithoutFormat ? "Enabled" : "Disabled");
        LOG_D("shaderStorageImageWriteWithoutFormat: %s", deviceFeatures.shaderStorageImageWriteWithoutFormat ? "Enabled" : "Disabled");
        LOG_D("shaderUniformBufferArrayDynamicIndexing: %s", deviceFeatures.shaderUniformBufferArrayDynamicIndexing ? "Enabled" : "Disabled");
        LOG_D("shaderSampledImageArrayDynamicIndexing: %s", deviceFeatures.shaderSampledImageArrayDynamicIndexing ? "Enabled" : "Disabled");
        LOG_D("shaderStorageBufferArrayDynamicIndexing: %s", deviceFeatures.shaderStorageBufferArrayDynamicIndexing ? "Enabled" : "Disabled");
        LOG_D("shaderStorageImageArrayDynamicIndexing: %s", deviceFeatures.shaderStorageImageArrayDynamicIndexing ? "Enabled" : "Disabled");
        LOG_D("shaderClipDistance: %s", deviceFeatures.shaderClipDistance ? "Enabled" : "Disabled");
        LOG_D("shaderCullDistance: %s", deviceFeatures.shaderCullDistance ? "Enabled" : "Disabled");
        LOG_D("shaderFloat64: %s", deviceFeatures.shaderFloat64 ? "Enabled" : "Disabled");
        LOG_D("shaderInt64: %s", deviceFeatures.shaderInt64 ? "Enabled" : "Disabled");
        LOG_D("shaderInt16: %s", deviceFeatures.shaderInt16 ? "Enabled" : "Disabled");
        LOG_D("shaderResourceResidency: %s", deviceFeatures.shaderResourceResidency ? "Enabled" : "Disabled");
        LOG_D("shaderResourceMinLod: %s", deviceFeatures.shaderResourceMinLod ? "Enabled" : "Disabled");
        LOG_D("sparseBinding: %s", deviceFeatures.sparseBinding ? "Enabled" : "Disabled");
        LOG_D("sparseResidencyBuffer: %s", deviceFeatures.sparseResidencyBuffer ? "Enabled" : "Disabled");
        LOG_D("sparseResidencyImage2D: %s", deviceFeatures.sparseResidencyImage2D ? "Enabled" : "Disabled");
        LOG_D("sparseResidencyImage3D: %s", deviceFeatures.sparseResidencyImage3D ? "Enabled" : "Disabled");
        LOG_D("sparseResidency2Samples: %s", deviceFeatures.sparseResidency2Samples ? "Enabled" : "Disabled");
        LOG_D("sparseResidency4Samples: %s", deviceFeatures.sparseResidency4Samples ? "Enabled" : "Disabled");
        LOG_D("sparseResidency8Samples: %s", deviceFeatures.sparseResidency8Samples ? "Enabled" : "Disabled");
        LOG_D("sparseResidency16Samples: %s", deviceFeatures.sparseResidency16Samples ? "Enabled" : "Disabled");
        LOG_D("sparseResidencyAliased: %s", deviceFeatures.sparseResidencyAliased ? "Enabled" : "Disabled");
        LOG_D("variableMultisampleRate: %s", deviceFeatures.variableMultisampleRate ? "Enabled" : "Disabled");
        LOG_D("inheritedQueries: %s", deviceFeatures.inheritedQueries ? "Enabled" : "Disabled");

        // 获取队列族属性
        auto queueFamilies = physicalDevice.getQueueFamilyProperties();
        LOG_D("Queue Family Properties:");
        for (size_t i = 0; i < queueFamilies.size(); ++i) {
            LOG_D("  Queue Family #%zu:", i);
            LOG_D("    Queue Count: %u", queueFamilies[i].queueCount);
            LOG_D("    Queue Flags: %s", vk::to_string(queueFamilies[i].queueFlags).c_str());
            LOG_D("    Timestamp Valid Bits: %u", queueFamilies[i].timestampValidBits);
            LOG_D("    Min Image Transfer Granularity: (%u, %u, %u)",
                  queueFamilies[i].minImageTransferGranularity.width,
                  queueFamilies[i].minImageTransferGranularity.height,
                  queueFamilies[i].minImageTransferGranularity.depth);
        }

        // 获取交换链支持详情并直接打印
        auto capabilities = physicalDevice.getSurfaceCapabilitiesKHR(surface);
        LOG_D("Surface Capabilities:");
        LOG_D("  minImageCount: %u", capabilities.minImageCount);
        LOG_D("  maxImageCount: %u", capabilities.maxImageCount);
        LOG_D("  currentExtent: (%u, %u)", capabilities.currentExtent.width, capabilities.currentExtent.height);
        LOG_D("  minImageExtent: (%u, %u)", capabilities.minImageExtent.width, capabilities.minImageExtent.height);
        LOG_D("  maxImageExtent: (%u, %u)", capabilities.maxImageExtent.width, capabilities.maxImageExtent.height);
        LOG_D("  maxImageArrayLayers: %u", capabilities.maxImageArrayLayers);
        LOG_D("  supportedTransforms: %s", vk::to_string(capabilities.supportedTransforms).c_str());
        LOG_D("  currentTransform: %s", vk::to_string(capabilities.currentTransform).c_str());
        LOG_D("  supportedCompositeAlpha: %s", vk::to_string(capabilities.supportedCompositeAlpha).c_str());
        LOG_D("  supportedUsageFlags: %s", vk::to_string(capabilities.supportedUsageFlags).c_str());

        // 打印格式信息
        auto formats = physicalDevice.getSurfaceFormatsKHR(surface);
        LOG_D("Surface Formats:");
        for (const auto &format: formats) {
            LOG_D("  Format: %s, ColorSpace: %s",
                  vk::to_string(format.format).c_str(),
                  vk::to_string(format.colorSpace).c_str());
        }

        // 打印呈现模式信息
        auto presentModes = physicalDevice.getSurfacePresentModesKHR(surface);
        LOG_D("Surface Present Modes:");
        for (const auto &presentMode: presentModes) {
            LOG_D("  PresentMode: %s", vk::to_string(presentMode).c_str());
        }

        // 获取设备支持的扩展
        auto extensions = physicalDevice.enumerateDeviceExtensionProperties();
        LOG_D("Supported Extensions:");
        for (const auto &ext: extensions) {
            LOG_D("  %s (version %u)", ext.extensionName.data(), ext.specVersion);
        }

        // 获取设备的内存属性
        auto memoryProperties = physicalDevice.getMemoryProperties();
        LOG_D("Memory Heaps:");
        for (uint32_t i = 0; i < memoryProperties.memoryHeapCount; ++i) {
            LOG_D("  Heap #%u: Size = %s, Flags = %s", i,
                  VulkanDevice::formatDeviceSize(memoryProperties.memoryHeaps[i].size).c_str(),
                  vk::to_string(memoryProperties.memoryHeaps[i].flags).c_str());
        }
        LOG_D("Memory Types:");
        for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; ++i) {
            LOG_D("  Type #%u: HeapIndex = %u, Flags = %s", i,
                  memoryProperties.memoryTypes[i].heapIndex,
                  vk::to_string(memoryProperties.memoryTypes[i].propertyFlags).c_str());
        }
    }

    // 转换 SampleCountFlags 为字符串
    std::string VulkanDevice::sampleCountFlagsToString(vk::SampleCountFlags flags) {
        std::string result;

        if (flags & vk::SampleCountFlagBits::e1) result += "1 ";
        if (flags & vk::SampleCountFlagBits::e2) result += "2 ";
        if (flags & vk::SampleCountFlagBits::e4) result += "4 ";
        if (flags & vk::SampleCountFlagBits::e8) result += "8 ";
        if (flags & vk::SampleCountFlagBits::e16) result += "16 ";
        if (flags & vk::SampleCountFlagBits::e32) result += "32 ";
        if (flags & vk::SampleCountFlagBits::e64) result += "64 ";

        if (result.empty()) return "None";
        return result;
    }

    // 将 vk::DeviceSize 转换为更易读的单位
    std::string VulkanDevice::formatDeviceSize(vk::DeviceSize size) {
        constexpr vk::DeviceSize KB = 1024;
        constexpr vk::DeviceSize MB = KB * 1024;
        constexpr vk::DeviceSize GB = MB * 1024;

        if (size >= GB) {
            return std::to_string(size / GB) + " GB";
        } else if (size >= MB) {
            return std::to_string(size / MB) + " MB";
        } else if (size >= KB) {
            return std::to_string(size / KB) + " KB";
        } else {
            return std::to_string(size) + " bytes";
        }
    }

    vk::ShaderModule VulkanDevice::createShaderModule(const std::vector<char> &code) {
        vk::ShaderModuleCreateInfo createInfo;
        createInfo.setCodeSize(code.size())
                .setPCode(reinterpret_cast<const uint32_t *>(code.data()));

        return mDevice.createShaderModule(createInfo);
    }

    bool QueueFamilyIndices::isComplete() {
        return graphicQueueFamilyIndex.has_value() && presentQueueFamilyIndex.has_value();
    }
} // engine