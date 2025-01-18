//
// Created by leixing on 2025/1/14.
//
#pragma once

#include "engine/VulkanVertexAttribute.h"
#include <vector>

namespace engine {

    class VulkanVertex {
    public:
        uint32_t binding;
        uint32_t size;
        std::vector<VulkanVertexAttribute> attributes;
    };

} // engine
