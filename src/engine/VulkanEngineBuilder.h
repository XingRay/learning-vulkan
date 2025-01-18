//
// Created by leixing on 2025/1/8.
//

#pragma once

#include <vector>
#include <memory>
#include <functional>

#include "engine/common/StringListSelector.h"

#include "engine/VulkanGraphicsEngineBuilder.h"
#include "engine/VulkanComputerEngineBuilder.h"

#ifndef APPLICATION_NAME
#define APPLICATION_NAME "3d_application"
#endif

#ifndef ENGINE_NAME
#define ENGINE_NAME "3d_engine"
#endif

namespace engine {

    class VulkanEngineBuilder {
    private:
        std::unique_ptr<common::ListSelector<std::string>> mExtensionsSelector;
        std::unique_ptr<common::ListSelector<std::string>> mLayersSelector;

        std::string mApplicationName = APPLICATION_NAME;
        uint32_t mApplicationVersion = VK_MAKE_VERSION(1, 0, 0);

        std::string mEngineName = ENGINE_NAME;
        uint32_t mEngineVersion = VK_MAKE_VERSION(1, 0, 0);

    public:
        VulkanEngineBuilder &extensions(const std::vector<std::string> required, const std::vector<std::string> optional = {});

        VulkanEngineBuilder &extensionsSelector(std::unique_ptr<common::ListSelector<std::string>> &selector);

        VulkanEngineBuilder &extensionsSelector(std::function<std::vector<std::string>(const std::vector<std::string> &)> selector);

        VulkanEngineBuilder &layers(const std::vector<std::string> required, const std::vector<std::string> optional = {});

        VulkanEngineBuilder &layersSelector(std::unique_ptr<common::ListSelector<std::string>> &selector);

        VulkanEngineBuilder &layersSelector(std::function<std::vector<std::string>(const std::vector<std::string> &)> selector);

        VulkanEngineBuilder &applicationName(const std::string &applicationName);

        VulkanEngineBuilder &applicationVersion(uint32_t version);

        VulkanEngineBuilder &applicationVersion(const std::string &version);

        VulkanEngineBuilder &engineName(const std::string &engineName);

        VulkanEngineBuilder &engineVersion(uint32_t version);

        VulkanEngineBuilder &engineVersion(const std::string &version);

        VulkanGraphicsEngineBuilder asGraphics();

        VulkanComputerEngineBuilder asComputer();

    private:
        std::unique_ptr<VulkanInstance> buildInstance();
    };

} // engine
