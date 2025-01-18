//
// Created by leixing on 2025/1/10.
//

#pragma once

#include <vector>

#include "vulkan/vulkan.hpp"

#include "engine/vulkan_wrapper/VulkanInstance.h"


namespace engine {

    class VulkanComputerEngineBuilder {
    private:
        std::unique_ptr<VulkanInstance> mInstance;

    public:
        explicit VulkanComputerEngineBuilder(std::unique_ptr<VulkanInstance> instance);

        ~VulkanComputerEngineBuilder();
    };

} // engine
