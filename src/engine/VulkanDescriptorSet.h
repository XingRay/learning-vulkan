//
// Created by leixing on 2025/1/15.
//

#pragma once

#include <cstdint>
#include <vector>

#include "engine/VulkanDescriptor.h"

namespace engine {

    class VulkanDescriptorSet {
    public:
        uint32_t set;
        std::vector<VulkanDescriptor> descriptors;
    };

} // engine
