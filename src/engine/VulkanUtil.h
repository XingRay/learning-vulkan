//
// Created by leixing on 2024/12/30.
//

#pragma once

#include "vulkan/vulkan.hpp"
#include "engine/vulkan_wrapper/VulkanDevice.h"
#include "engine/VulkanPhysicalDeviceCandidate.h"

namespace engine {

    class VulkanUtil {
    public:

        static QueueFamilyIndices findQueueFamilies(const vk::PhysicalDevice &physicalDevice, const vk::SurfaceKHR &surface);

        static bool isDeviceSupportedRequiredDeviceExtensions(const vk::PhysicalDevice &device,
                                                              const std::vector<const char *> &requiredDeviceExtensions);

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

        static std::string sampleCountFlagsToString(vk::SampleCountFlags flags);

        static std::string formatDeviceSize(vk::DeviceSize size);

        static void recordTransitionImageLayoutCommand(const vk::CommandBuffer &commandBuffer,
                                                       vk::Image image,
                                                       vk::Format format,
                                                       vk::ImageLayout oldImageLayout,
                                                       vk::ImageLayout newImageLayout,
                                                       uint32_t mipLevels);

        static bool hasStencilComponent(vk::Format format);

        static vk::Format findDepthFormat(const vk::PhysicalDevice &physicalDevice);

        static vk::Format findSupportedFormat(const vk::PhysicalDevice &physicalDevice, const std::vector<vk::Format> &candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features);

        static vk::SampleCountFlagBits uint32ToSampleCountFlagBits(uint32_t sampleCount);

        static uint32_t getFormatSize(vk::Format format);

        static vk::DescriptorPoolSize &getOrCreateDescriptorPoolSize(std::vector<vk::DescriptorPoolSize> &descriptorPoolSizes, vk::DescriptorType type);
    };

} // engine
