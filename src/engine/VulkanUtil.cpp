//
// Created by leixing on 2024/12/30.
//

#include "engine/VulkanUtil.h"
#include "engine/Log.h"
#include <set>
#include <string>

namespace engine {

    QueueFamilyIndices VulkanUtil::findQueueFamilies(const vk::PhysicalDevice &physicalDevice, const vk::SurfaceKHR &surface) {
        QueueFamilyIndices indices;
        auto queueFamilyProperties = physicalDevice.getQueueFamilyProperties();

        for (int i = 0; i < queueFamilyProperties.size(); i++) {
            const auto &queueFamilyProperty = queueFamilyProperties[i];
            const vk::QueueFlags &queueFlags = queueFamilyProperty.queueFlags;

            if (queueFlags & vk::QueueFlagBits::eGraphics) {
                LOG_D("graphicQueueFamily found, index:%d", i);
                indices.graphicQueueFamilyIndex = i;
            }

            if (physicalDevice.getSurfaceSupportKHR(i, surface)) {
                LOG_D("presentQueueFamily found, index:%d", i);
                indices.presentQueueFamilyIndex = i;
            }

            if (indices.isComplete()) {
                break;
            }
        }

        return indices;
    }

    bool VulkanUtil::isDeviceSupportedRequiredDeviceExtensions(const vk::PhysicalDevice &device,
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


    std::pair<vk::Buffer, vk::DeviceMemory> VulkanUtil::createBuffer(const VulkanDevice &vulkanDevice, vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties) {
        const vk::Device device = vulkanDevice.getDevice();
        const vk::PhysicalDevice physicalDevice = vulkanDevice.getPhysicalDevice();

        vk::BufferCreateInfo bufferCreateInfo{};
        bufferCreateInfo.setSize(size)
                .setUsage(usage)
                .setSharingMode(vk::SharingMode::eExclusive);

        vk::Buffer buffer = device.createBuffer(bufferCreateInfo);
        vk::MemoryRequirements memoryRequirements = device.getBufferMemoryRequirements(buffer);
        vk::PhysicalDeviceMemoryProperties memoryProperties = physicalDevice.getMemoryProperties();

        uint32_t memoryType = findMemoryType(memoryProperties, memoryRequirements.memoryTypeBits, properties);
        vk::MemoryAllocateInfo memoryAllocateInfo{};
        memoryAllocateInfo
                .setAllocationSize(memoryRequirements.size)
                .setMemoryTypeIndex(memoryType);

        vk::DeviceMemory bufferMemory = device.allocateMemory(memoryAllocateInfo);
        device.bindBufferMemory(buffer, bufferMemory, 0);

        return {buffer, bufferMemory};
    }

    uint32_t VulkanUtil::findMemoryType(const vk::PhysicalDeviceMemoryProperties &memoryProperties, uint32_t typeFilter, vk::MemoryPropertyFlags properties) {
        for (int i = 0; i < memoryProperties.memoryTypeCount; i++) {
            if (typeFilter & (1 << i) && ((memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)) {
                return i;
            }
        }

        throw std::runtime_error("failed to find suitable memory type !");
    }

    void VulkanUtil::recordCopyBufferCommand(const vk::CommandBuffer &commandBuffer,
                                             const vk::Buffer &srcBuffer,
                                             const vk::Buffer &dstBuffer,
                                             vk::DeviceSize size) {
        vk::BufferCopy bufferCopy;
        bufferCopy.setSrcOffset(0)
                .setDstOffset(0)
                .setSize(size);

        commandBuffer.copyBuffer(srcBuffer, dstBuffer, bufferCopy);
    }

    std::pair<vk::Image, vk::DeviceMemory> VulkanUtil::createImage(
            const vk::Device &device,
            vk::PhysicalDeviceMemoryProperties properties,
            uint32_t width,
            uint32_t height,
            uint32_t mipLevels,
            vk::SampleCountFlagBits numSamples,
            vk::Format format,
            vk::ImageTiling imageTiling,
            vk::ImageUsageFlags imageUsage,
            vk::MemoryPropertyFlags memoryProperty) {

        vk::Extent3D extent;
        extent.setWidth(width)
                .setHeight(height)
                .setDepth(1);
        vk::ImageCreateInfo imageCreateInfo;
        imageCreateInfo.setImageType(vk::ImageType::e2D)
                .setExtent(extent)
                .setMipLevels(mipLevels)
                .setArrayLayers(1)
                .setFormat(format)
                .setTiling(imageTiling)
                .setInitialLayout(vk::ImageLayout::eUndefined)
                .setUsage(imageUsage)
                .setSharingMode(vk::SharingMode::eExclusive)
                .setSamples(numSamples)
                .setFlags(vk::ImageCreateFlags{});

        vk::Image image = device.createImage(imageCreateInfo);

        vk::MemoryRequirements memoryRequirements = device.getImageMemoryRequirements(image);

        vk::MemoryAllocateInfo memoryAllocateInfo;

        uint32_t memoryType = VulkanUtil::findMemoryType(properties, memoryRequirements.memoryTypeBits, memoryProperty);
        memoryAllocateInfo
                .setAllocationSize(memoryRequirements.size)
                .setMemoryTypeIndex(memoryType);

        vk::DeviceMemory imageMemory = device.allocateMemory(memoryAllocateInfo);

        device.bindImageMemory(image, imageMemory, 0);

        return {image, imageMemory};
    }

    vk::ImageView VulkanUtil::createImageView(const vk::Device &device, const vk::Image &image, vk::Format format, vk::ImageAspectFlags imageAspect, uint32_t mipLevels) {
        vk::ImageViewCreateInfo imageViewCreateInfo{};
        imageViewCreateInfo.setImage(image)
                .setViewType(vk::ImageViewType::e2D)
                .setFormat(format);
//            .setSubresourceRange(imageSubresourceRange)
//            .setComponents(componentMapping);

        vk::ImageSubresourceRange &imageSubresourceRange = imageViewCreateInfo.subresourceRange;
        imageSubresourceRange.setAspectMask(imageAspect)
                .setBaseMipLevel(0)
                .setLevelCount(mipLevels)
                .setBaseArrayLayer(0)
                .setLayerCount(1);

        vk::ComponentMapping &componentMapping = imageViewCreateInfo.components;
        componentMapping.setR(vk::ComponentSwizzle::eIdentity)
                .setG(vk::ComponentSwizzle::eIdentity)
                .setB(vk::ComponentSwizzle::eIdentity)
                .setA(vk::ComponentSwizzle::eIdentity);

        return device.createImageView(imageViewCreateInfo);
    }

    // 转换 SampleCountFlags 为字符串
    std::string VulkanUtil::sampleCountFlagsToString(vk::SampleCountFlags flags) {
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
    std::string VulkanUtil::formatDeviceSize(vk::DeviceSize size) {
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

    void VulkanUtil::recordTransitionImageLayoutCommand(const vk::CommandBuffer &commandBuffer,
                                                        vk::Image image,
                                                        vk::Format format,
                                                        vk::ImageLayout oldImageLayout,
                                                        vk::ImageLayout newImageLayout,
                                                        uint32_t mipLevels) {

        vk::ImageSubresourceRange imageSubresourceRange;
        imageSubresourceRange
                .setBaseMipLevel(0)
                .setLevelCount(mipLevels)
                .setBaseArrayLayer(0)
                .setLayerCount(1);

        // 注意这里一定要是 vk::ImageLayout::eDepthStencilAttachmentOptimal ， 写成了 vk::ImageLayout::eStencilAttachmentOptimal 后面会报警告
        if (newImageLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal) {
            if (hasStencilComponent(format)) {
                imageSubresourceRange.setAspectMask(vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil);
            } else {
                imageSubresourceRange.setAspectMask(vk::ImageAspectFlagBits::eDepth);
            }
        } else {
            imageSubresourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor);
        }


        vk::ImageMemoryBarrier imageMemoryBarrier;
        imageMemoryBarrier.setOldLayout(oldImageLayout)
                .setNewLayout(newImageLayout)
                .setSrcQueueFamilyIndex(vk::QueueFamilyIgnored)
                .setDstQueueFamilyIndex(vk::QueueFamilyIgnored)
                .setImage(image)
                .setSubresourceRange(imageSubresourceRange)
                .setSrcAccessMask(vk::AccessFlags{})
                .setDstAccessMask(vk::AccessFlags{});

        vk::PipelineStageFlags sourceStage;
        vk::PipelineStageFlags destinationStage;

        if (oldImageLayout == vk::ImageLayout::eUndefined && newImageLayout == vk::ImageLayout::eTransferDstOptimal) {
            imageMemoryBarrier
                    .setSrcAccessMask(vk::AccessFlags{})
                    .setDstAccessMask(vk::AccessFlagBits::eTransferWrite);

            sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
            destinationStage = vk::PipelineStageFlagBits::eTransfer;
        } else if (oldImageLayout == vk::ImageLayout::eTransferDstOptimal && newImageLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {
            imageMemoryBarrier
                    .setSrcAccessMask(vk::AccessFlagBits::eTransferWrite)
                    .setDstAccessMask(vk::AccessFlagBits::eShaderRead);

            sourceStage = vk::PipelineStageFlagBits::eTransfer;
            destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
        } else if (oldImageLayout == vk::ImageLayout::eUndefined && newImageLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal) {
            imageMemoryBarrier
                    .setSrcAccessMask(vk::AccessFlags{})
                    .setDstAccessMask(vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite);

            sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
            destinationStage = vk::PipelineStageFlagBits::eEarlyFragmentTests;
        } else {
            throw std::runtime_error("unsupported layout transition!");
        }

        commandBuffer.pipelineBarrier(sourceStage,
                                      destinationStage,
                                      vk::DependencyFlags{},
                // 内存屏障
                                      {},
                // 缓冲区内存屏障
                                      {},
                // 图像内存屏障
                                      {imageMemoryBarrier});


    }

    bool VulkanUtil::hasStencilComponent(vk::Format format) {
        return format == vk::Format::eD32SfloatS8Uint || format == vk::Format::eD24UnormS8Uint;
    }

    vk::Format VulkanUtil::findDepthFormat(const vk::PhysicalDevice &physicalDevice) {
        return VulkanUtil::findSupportedFormat(
                physicalDevice,
                {vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint},
                vk::ImageTiling::eOptimal,
                vk::FormatFeatureFlagBits::eDepthStencilAttachment
        );
    }

    vk::Format VulkanUtil::findSupportedFormat(const vk::PhysicalDevice &physicalDevice, const std::vector<vk::Format> &candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features) {
        for (const auto &format: candidates) {
            vk::FormatProperties properties = physicalDevice.getFormatProperties(format);
            if (tiling == vk::ImageTiling::eLinear && (properties.linearTilingFeatures & features) == features) {
                return format;
            } else if (tiling == vk::ImageTiling::eOptimal && (properties.optimalTilingFeatures & features) == features) {
                return format;
            }
        }

        throw std::runtime_error("failed to find supported format !");
    }

    vk::SampleCountFlagBits VulkanUtil::uint32ToSampleCountFlagBits(uint32_t sampleCount) {
        switch (sampleCount) {
            case 1:
                return vk::SampleCountFlagBits::e1;
            case 2:
                return vk::SampleCountFlagBits::e2;
            case 4:
                return vk::SampleCountFlagBits::e4;
            case 8:
                return vk::SampleCountFlagBits::e8;
            case 16:
                return vk::SampleCountFlagBits::e16;
            case 32:
                return vk::SampleCountFlagBits::e32;
            case 64:
                return vk::SampleCountFlagBits::e64;
            default:
                throw std::invalid_argument("Unsupported sample count: " + std::to_string(sampleCount));
        }
    }


    uint32_t VulkanUtil::getFormatSize(vk::Format format) {
        switch (format) {
            // 8-bit formats (1 byte per component)
            case vk::Format::eR4G4UnormPack8:
            case vk::Format::eR8Unorm:
            case vk::Format::eR8Snorm:
            case vk::Format::eR8Uscaled:
            case vk::Format::eR8Sscaled:
            case vk::Format::eR8Uint:
            case vk::Format::eR8Sint:
            case vk::Format::eR8Srgb:
            case vk::Format::eS8Uint:
                return 1;

                // 16-bit formats (2 bytes per component)
            case vk::Format::eR4G4B4A4UnormPack16:
            case vk::Format::eB4G4R4A4UnormPack16:
            case vk::Format::eR5G6B5UnormPack16:
            case vk::Format::eB5G6R5UnormPack16:
            case vk::Format::eR5G5B5A1UnormPack16:
            case vk::Format::eB5G5R5A1UnormPack16:
            case vk::Format::eA1R5G5B5UnormPack16:
            case vk::Format::eR8G8Unorm:
            case vk::Format::eR8G8Snorm:
            case vk::Format::eR8G8Uscaled:
            case vk::Format::eR8G8Sscaled:
            case vk::Format::eR8G8Uint:
            case vk::Format::eR8G8Sint:
            case vk::Format::eR8G8Srgb:
            case vk::Format::eR16Unorm:
            case vk::Format::eR16Snorm:
            case vk::Format::eR16Uscaled:
            case vk::Format::eR16Sscaled:
            case vk::Format::eR16Uint:
            case vk::Format::eR16Sint:
            case vk::Format::eR16Sfloat:
            case vk::Format::eD16Unorm:
                return 2;

                // 24-bit formats (3 bytes per component)
            case vk::Format::eR8G8B8Unorm:
            case vk::Format::eR8G8B8Snorm:
            case vk::Format::eR8G8B8Uscaled:
            case vk::Format::eR8G8B8Sscaled:
            case vk::Format::eR8G8B8Uint:
            case vk::Format::eR8G8B8Sint:
            case vk::Format::eR8G8B8Srgb:
            case vk::Format::eB8G8R8Unorm:
            case vk::Format::eB8G8R8Snorm:
            case vk::Format::eB8G8R8Uscaled:
            case vk::Format::eB8G8R8Sscaled:
            case vk::Format::eB8G8R8Uint:
            case vk::Format::eB8G8R8Sint:
            case vk::Format::eB8G8R8Srgb:
                return 3;

                // 32-bit formats (4 bytes per component)
            case vk::Format::eR8G8B8A8Unorm:
            case vk::Format::eR8G8B8A8Snorm:
            case vk::Format::eR8G8B8A8Uscaled:
            case vk::Format::eR8G8B8A8Sscaled:
            case vk::Format::eR8G8B8A8Uint:
            case vk::Format::eR8G8B8A8Sint:
            case vk::Format::eR8G8B8A8Srgb:
            case vk::Format::eB8G8R8A8Unorm:
            case vk::Format::eB8G8R8A8Snorm:
            case vk::Format::eB8G8R8A8Uscaled:
            case vk::Format::eB8G8R8A8Sscaled:
            case vk::Format::eB8G8R8A8Uint:
            case vk::Format::eB8G8R8A8Sint:
            case vk::Format::eB8G8R8A8Srgb:
            case vk::Format::eA8B8G8R8UnormPack32:
            case vk::Format::eA8B8G8R8SnormPack32:
            case vk::Format::eA8B8G8R8UscaledPack32:
            case vk::Format::eA8B8G8R8SscaledPack32:
            case vk::Format::eA8B8G8R8UintPack32:
            case vk::Format::eA8B8G8R8SintPack32:
            case vk::Format::eA8B8G8R8SrgbPack32:
            case vk::Format::eA2R10G10B10UnormPack32:
            case vk::Format::eA2R10G10B10SnormPack32:
            case vk::Format::eA2R10G10B10UscaledPack32:
            case vk::Format::eA2R10G10B10SscaledPack32:
            case vk::Format::eA2R10G10B10UintPack32:
            case vk::Format::eA2R10G10B10SintPack32:
            case vk::Format::eA2B10G10R10UnormPack32:
            case vk::Format::eA2B10G10R10SnormPack32:
            case vk::Format::eA2B10G10R10UscaledPack32:
            case vk::Format::eA2B10G10R10SscaledPack32:
            case vk::Format::eA2B10G10R10UintPack32:
            case vk::Format::eA2B10G10R10SintPack32:
            case vk::Format::eR16G16Unorm:
            case vk::Format::eR16G16Snorm:
            case vk::Format::eR16G16Uscaled:
            case vk::Format::eR16G16Sscaled:
            case vk::Format::eR16G16Uint:
            case vk::Format::eR16G16Sint:
            case vk::Format::eR16G16Sfloat:
            case vk::Format::eR32Uint:
            case vk::Format::eR32Sint:
            case vk::Format::eR32Sfloat:
            case vk::Format::eX8D24UnormPack32:
            case vk::Format::eD32Sfloat:
            case vk::Format::eD24UnormS8Uint:
            case vk::Format::eD32SfloatS8Uint:
                return 4;

                // 48-bit formats (6 bytes per component)
            case vk::Format::eR16G16B16Unorm:
            case vk::Format::eR16G16B16Snorm:
            case vk::Format::eR16G16B16Uscaled:
            case vk::Format::eR16G16B16Sscaled:
            case vk::Format::eR16G16B16Uint:
            case vk::Format::eR16G16B16Sint:
            case vk::Format::eR16G16B16Sfloat:
                return 6;

                // 64-bit formats (8 bytes per component)
            case vk::Format::eR16G16B16A16Unorm:
            case vk::Format::eR16G16B16A16Snorm:
            case vk::Format::eR16G16B16A16Uscaled:
            case vk::Format::eR16G16B16A16Sscaled:
            case vk::Format::eR16G16B16A16Uint:
            case vk::Format::eR16G16B16A16Sint:
            case vk::Format::eR16G16B16A16Sfloat:
            case vk::Format::eR32G32Uint:
            case vk::Format::eR32G32Sint:
            case vk::Format::eR32G32Sfloat:
            case vk::Format::eR64Uint:
            case vk::Format::eR64Sint:
            case vk::Format::eR64Sfloat:
                return 8;

                // 96-bit formats (12 bytes per component)
            case vk::Format::eR32G32B32Uint:
            case vk::Format::eR32G32B32Sint:
            case vk::Format::eR32G32B32Sfloat:
                return 12;

                // 128-bit formats (16 bytes per component)
            case vk::Format::eR32G32B32A32Uint:
            case vk::Format::eR32G32B32A32Sint:
            case vk::Format::eR32G32B32A32Sfloat:
            case vk::Format::eR64G64Uint:
            case vk::Format::eR64G64Sint:
            case vk::Format::eR64G64Sfloat:
            case vk::Format::eR64G64B64Uint:
            case vk::Format::eR64G64B64Sint:
            case vk::Format::eR64G64B64Sfloat:
            case vk::Format::eR64G64B64A64Uint:
            case vk::Format::eR64G64B64A64Sint:
            case vk::Format::eR64G64B64A64Sfloat:
                return 16;

                // Compressed formats (block-based, size depends on block dimensions)
            case vk::Format::eBc1RgbUnormBlock:
            case vk::Format::eBc1RgbSrgbBlock:
            case vk::Format::eBc1RgbaUnormBlock:
            case vk::Format::eBc1RgbaSrgbBlock:
            case vk::Format::eBc2UnormBlock:
            case vk::Format::eBc2SrgbBlock:
            case vk::Format::eBc3UnormBlock:
            case vk::Format::eBc3SrgbBlock:
            case vk::Format::eBc4UnormBlock:
            case vk::Format::eBc4SnormBlock:
            case vk::Format::eBc5UnormBlock:
            case vk::Format::eBc5SnormBlock:
            case vk::Format::eBc6HUfloatBlock:
            case vk::Format::eBc6HSfloatBlock:
            case vk::Format::eBc7UnormBlock:
            case vk::Format::eBc7SrgbBlock:
            case vk::Format::eEtc2R8G8B8UnormBlock:
            case vk::Format::eEtc2R8G8B8SrgbBlock:
            case vk::Format::eEtc2R8G8B8A1UnormBlock:
            case vk::Format::eEtc2R8G8B8A1SrgbBlock:
            case vk::Format::eEtc2R8G8B8A8UnormBlock:
            case vk::Format::eEtc2R8G8B8A8SrgbBlock:
            case vk::Format::eEacR11UnormBlock:
            case vk::Format::eEacR11SnormBlock:
            case vk::Format::eEacR11G11UnormBlock:
            case vk::Format::eEacR11G11SnormBlock:
            case vk::Format::eAstc4x4UnormBlock:
            case vk::Format::eAstc4x4SrgbBlock:
            case vk::Format::eAstc5x4UnormBlock:
            case vk::Format::eAstc5x4SrgbBlock:
            case vk::Format::eAstc5x5UnormBlock:
            case vk::Format::eAstc5x5SrgbBlock:
            case vk::Format::eAstc6x5UnormBlock:
            case vk::Format::eAstc6x5SrgbBlock:
            case vk::Format::eAstc6x6UnormBlock:
            case vk::Format::eAstc6x6SrgbBlock:
            case vk::Format::eAstc8x5UnormBlock:
            case vk::Format::eAstc8x5SrgbBlock:
            case vk::Format::eAstc8x6UnormBlock:
            case vk::Format::eAstc8x6SrgbBlock:
            case vk::Format::eAstc8x8UnormBlock:
            case vk::Format::eAstc8x8SrgbBlock:
            case vk::Format::eAstc10x5UnormBlock:
            case vk::Format::eAstc10x5SrgbBlock:
            case vk::Format::eAstc10x6UnormBlock:
            case vk::Format::eAstc10x6SrgbBlock:
            case vk::Format::eAstc10x8UnormBlock:
            case vk::Format::eAstc10x8SrgbBlock:
            case vk::Format::eAstc10x10UnormBlock:
            case vk::Format::eAstc10x10SrgbBlock:
            case vk::Format::eAstc12x10UnormBlock:
            case vk::Format::eAstc12x10SrgbBlock:
            case vk::Format::eAstc12x12UnormBlock:
            case vk::Format::eAstc12x12SrgbBlock:
            case vk::Format::ePvrtc12BppUnormBlockIMG:
            case vk::Format::ePvrtc14BppUnormBlockIMG:
            case vk::Format::ePvrtc22BppUnormBlockIMG:
            case vk::Format::ePvrtc24BppUnormBlockIMG:
            case vk::Format::ePvrtc12BppSrgbBlockIMG:
            case vk::Format::ePvrtc14BppSrgbBlockIMG:
            case vk::Format::ePvrtc22BppSrgbBlockIMG:
            case vk::Format::ePvrtc24BppSrgbBlockIMG:
                throw std::runtime_error("Compressed formats are not supported for size calculation");

                // Other formats (e.g., multi-plane or special formats)
            case vk::Format::eG8B8G8R8422Unorm:
            case vk::Format::eB8G8R8G8422Unorm:
            case vk::Format::eG8B8R83Plane420Unorm:
            case vk::Format::eG8B8R82Plane420Unorm:
            case vk::Format::eG8B8R83Plane422Unorm:
            case vk::Format::eG8B8R82Plane422Unorm:
            case vk::Format::eG8B8R83Plane444Unorm:
            case vk::Format::eG16B16G16R16422Unorm:
            case vk::Format::eB16G16R16G16422Unorm:
            case vk::Format::eG16B16R163Plane420Unorm:
            case vk::Format::eG16B16R162Plane420Unorm:
            case vk::Format::eG16B16R163Plane422Unorm:
            case vk::Format::eG16B16R162Plane422Unorm:
            case vk::Format::eG16B16R163Plane444Unorm:
                throw std::runtime_error("Multi-plane or special formats are not supported for size calculation");

                // Undefined format
            case vk::Format::eUndefined:
                throw std::runtime_error("Undefined format has no size");

            default:
                throw std::runtime_error("Unsupported format");
        }
    }

    vk::DescriptorPoolSize &VulkanUtil::getOrCreateDescriptorPoolSize(std::vector<vk::DescriptorPoolSize> &descriptorPoolSizes, vk::DescriptorType type) {
        // 查找指定类型的 DescriptorPoolSize
        auto it = std::find_if(descriptorPoolSizes.begin(), descriptorPoolSizes.end(), [type](const vk::DescriptorPoolSize &poolSize) {
            return poolSize.type == type;
        });

        // 如果找到，返回该对象的引用
        if (it != descriptorPoolSizes.end()) {
            return *it;
        }

        // 如果没有找到，创建一个新的 DescriptorPoolSize 并添加到列表中
        descriptorPoolSizes.push_back({type, 0}); // 初始 descriptorCount 为 0
        return descriptorPoolSizes.back();
    }

} // engine