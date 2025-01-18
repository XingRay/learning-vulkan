//
// Created by leixing on 2025/1/9.
//

#pragma once

#include "vulkan/vulkan.hpp"
#include <optional>

namespace engine {

    class QueueFamilyIndices {
    public:
        std::optional<uint32_t> graphicQueueFamilyIndex;
        std::optional<uint32_t> presentQueueFamilyIndex;

    public:
        bool isComplete();
    };

} // engine
