//
// Created by leixing on 2025/1/8.
//

#include "engine/VulkanEngineBuilder.h"
#include "common/StringUtil.h"

namespace engine {

    VulkanEngineBuilder &VulkanEngineBuilder::extensions(const std::vector<std::string> required, const std::vector<std::string> optional) {
        mExtensionsSelector = std::make_unique<common::RequiredAndOptionalStringListSelector>(required, optional);
        return *this;
    }

    VulkanEngineBuilder &VulkanEngineBuilder::extensionsSelector(std::unique_ptr<common::ListSelector<std::string>> &selector) {
        mExtensionsSelector = std::move(selector);
        return *this;
    }

    VulkanEngineBuilder &VulkanEngineBuilder::extensionsSelector(std::function<std::vector<std::string>(const std::vector<std::string> &)> selector) {
        mExtensionsSelector = std::make_unique<common::LambdaStringListSelector>(selector);
        return *this;
    }

    VulkanEngineBuilder &VulkanEngineBuilder::layers(const std::vector<std::string> required, const std::vector<std::string> optional) {
        mLayersSelector = std::make_unique<common::RequiredAndOptionalStringListSelector>(required, optional);
        return *this;
    }

    VulkanEngineBuilder &VulkanEngineBuilder::layersSelector(std::unique_ptr<common::ListSelector<std::string>> &selector) {
        mLayersSelector = std::move(selector);
        return *this;
    }

    VulkanEngineBuilder &VulkanEngineBuilder::layersSelector(std::function<std::vector<std::string>(const std::vector<std::string> &)> selector) {
        mLayersSelector = std::make_unique<common::LambdaStringListSelector>(selector);
        return *this;
    }

    VulkanEngineBuilder &VulkanEngineBuilder::applicationName(const std::string &applicationName) {
        mApplicationName = applicationName;
        return *this;
    }

    VulkanEngineBuilder &VulkanEngineBuilder::applicationVersion(uint32_t version) {
        mApplicationVersion = version;
        return *this;
    }

    VulkanEngineBuilder &VulkanEngineBuilder::applicationVersion(const std::string &version) {
        std::vector<uint32_t> versionNumbers = common::StringUtil::parseVersion(version);

        if (versionNumbers.size() < 2) {
            throw std::invalid_argument("Invalid version format: at least major and minor versions are required");
        }

        if (versionNumbers.size() == 2) {
            versionNumbers.push_back(0);
        }

        mApplicationVersion = VK_MAKE_VERSION(versionNumbers[0], versionNumbers[1], versionNumbers[2]);

        return *this;
    }

    VulkanEngineBuilder &VulkanEngineBuilder::engineName(const std::string &engineName) {
        mEngineName = engineName;
        return *this;
    }

    VulkanEngineBuilder &VulkanEngineBuilder::engineVersion(uint32_t version) {
        mEngineVersion = version;
        return *this;
    }

    VulkanEngineBuilder &VulkanEngineBuilder::engineVersion(const std::string &version) {
        std::vector<uint32_t> versionNumbers = common::StringUtil::parseVersion(version);

        if (versionNumbers.size() < 2) {
            throw std::invalid_argument("Invalid version format: at least major and minor versions are required");
        }

        if (versionNumbers.size() == 2) {
            versionNumbers.push_back(0);
        }

        mEngineVersion = VK_MAKE_VERSION(versionNumbers[0], versionNumbers[1], versionNumbers[2]);

        return *this;
    }

    VulkanGraphicsEngineBuilder VulkanEngineBuilder::asGraphics() {
        std::unique_ptr<VulkanInstance> instance = buildInstance();
        return VulkanGraphicsEngineBuilder{std::move(instance)};
    }

    VulkanComputerEngineBuilder VulkanEngineBuilder::asComputer() {
        std::unique_ptr<VulkanInstance> instance = buildInstance();
        return VulkanComputerEngineBuilder(std::move(instance));
    }

    std::unique_ptr<VulkanInstance> VulkanEngineBuilder::buildInstance() {
        return std::make_unique<VulkanInstance>(mApplicationName, mApplicationVersion,
                                                mEngineName, mEngineVersion,
                                                *mExtensionsSelector, *mLayersSelector);
    }

} // engine