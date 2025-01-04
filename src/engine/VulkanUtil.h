//
// Created by leixing on 2024/12/30.
//

#ifndef VULKANDEMO_VULKANUTIL_H
#define VULKANDEMO_VULKANUTIL_H

#include "vulkan/vulkan.hpp"
#include "VulkanDevice.h"

namespace engine {

    class VulkanUtil {
    public:
        static std::pair<vk::Buffer, vk::DeviceMemory> createBuffer(const VulkanDevice &vulkanDevice, vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties);

        static uint32_t findMemoryType(const vk::PhysicalDeviceMemoryProperties &memoryProperties, uint32_t typeFilter, vk::MemoryPropertyFlags properties);

        static void recordCopyBufferCommand(const vk::CommandBuffer &commandBuffer, const vk::Buffer &srcBuffer, const vk::Buffer &dstBuffer, vk::DeviceSize size);

        static std::pair<vk::Image, vk::DeviceMemory> createImage(const vk::Device &device,
                                                                  vk::PhysicalDeviceMemoryProperties properties,
                                                                  uint32_t width,
                                                                  uint32_t height,
                                                                  uint32_t mipLevels,
                                                                  vk::SampleCountFlagBits numSamples,
                                                                  vk::Format format,
                                                                  vk::ImageTiling imageTiling,
                                                                  vk::ImageUsageFlags imageUsage,
                                                                  vk::MemoryPropertyFlags memoryProperty);

        static vk::ImageView createImageView(const vk::Device &device,
                                             const vk::Image &image,
                                             vk::Format format,
                                             vk::ImageAspectFlags
                                             imageAspect,
                                             uint32_t mipLevels);
    };

} // engine

#endif //VULKANDEMO_VULKANUTIL_H
