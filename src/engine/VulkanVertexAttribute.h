//
// Created by leixing on 2025/1/14.
//

#pragma once

#include "vulkan/vulkan.hpp"
#include <stdint.h>

namespace engine {

    class VulkanVertexAttribute {
    public:
        uint32_t binding;
        uint32_t location;
        vk::Format format;
        uint32_t offset;
    };

} // engine
