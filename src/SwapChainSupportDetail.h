//
// Created by leixing on 2024/5/14.
//

#pragma once

#include "vulkan/vulkan.hpp"
#include <vector>

struct SwapChainSupportDetail {
    vk::SurfaceCapabilitiesKHR capabilities;
    std::vector<vk::SurfaceFormatKHR> formats;
    std::vector<vk::PresentModeKHR> presentModes;
};