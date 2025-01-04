//
// Created by leixing on 2024/12/30.
//

#include "VulkanUtil.h"

namespace engine {

    std::pair<vk::Buffer, vk::DeviceMemory> VulkanUtil::createBuffer(const VulkanDevice &vulkanDevice, vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties) {
        const vk::Device device = vulkanDevice.getDevice();
        const vk::PhysicalDevice physicalDevice = vulkanDevice.getPhysicalDevice();

//        std::vector<uint32_t> queueIndices = vulkanDevice.getQueueFamilyIndices();
//        vk::SharingMode sharingMode;
//        if (queueIndices.size() == 1) {
//            sharingMode = vk::SharingMode::eExclusive;
//        } else {
//            sharingMode = vk::SharingMode::eConcurrent;
//        }

        vk::BufferCreateInfo bufferCreateInfo{};
        bufferCreateInfo.setSize(size)
                .setUsage(usage)
                .setSharingMode(vk::SharingMode::eExclusive);
//                .setSharingMode(sharingMode)
//                .setQueueFamilyIndices(queueIndices);

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

} // engine