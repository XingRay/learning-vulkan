//
// Created by leixing on 2025/1/10.
//

#pragma once

#include <memory>
#include <vector>

#include "vulkan/vulkan.hpp"

namespace engine {

    class VulkanPhysicalDeviceSurfaceSupport {
    public:
        uint32_t graphicQueueFamilyIndex;
        uint32_t presentQueueFamilyIndex;

        vk::SurfaceCapabilitiesKHR capabilities;
        std::vector<vk::SurfaceFormatKHR> formats;
        std::vector<vk::PresentModeKHR> presentModes;
    };

} // engine
