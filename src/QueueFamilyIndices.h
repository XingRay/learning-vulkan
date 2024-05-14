//
// Created by leixing on 2024/5/12.
//

#pragma once

#include <cstdint>
#include "vulkan/vulkan.hpp"
#include <optional>

class QueueFamilyIndices {

public:
    std::optional<uint32_t> graphicQueueFamily;
    std::optional<uint32_t> presentQueueFamily;

public:
    bool isComplete();
};