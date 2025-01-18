//
// Created by leixing on 2025/1/9.
//

#pragma once

#include "vulkan/vulkan.hpp"
#include <vector>

namespace engine {

    class SwapChainSupportDetail {
    public:
        vk::SurfaceCapabilitiesKHR capabilities;
        std::vector<vk::SurfaceFormatKHR> formats;
        std::vector<vk::PresentModeKHR> presentModes;
    };

} // engine
