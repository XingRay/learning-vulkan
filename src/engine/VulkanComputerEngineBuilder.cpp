//
// Created by leixing on 2025/1/10.
//

#include "VulkanComputerEngineBuilder.h"

/**
 * todo
 */
namespace engine {

    VulkanComputerEngineBuilder::VulkanComputerEngineBuilder(std::unique_ptr<VulkanInstance> instance) {
        mInstance = std::move(instance);
    }

    VulkanComputerEngineBuilder::~VulkanComputerEngineBuilder() = default;

} // engine