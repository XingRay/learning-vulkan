//
// Created by leixing on 2025/1/11.
//

#pragma once

#include <functional>

#include "engine/ShaderFormat.h"
#include "engine/VulkanGraphicsEngineBuilder.h"

#include "engine/VulkanVertex.h"
#include "engine/VulkanVertexConfigure.h"

#include "engine/VulkanDescriptorSet.h"
#include "engine/VulkanDescriptorSetConfigure.h"
#include "engine/VulkanPushConstant.h"

namespace engine {

    class VulkanGraphicsEngineBuilder;

    class VulkanVertexConfigure;

    class VulkanDescriptorSetConfigure;

    class VulkanShaderConfigure {
    private:

        VulkanGraphicsEngineBuilder &mBuilder;

        /**
         * shader code
         */
        std::vector<char> mVertexShaderCode;
        std::vector<char> mFragmentShaderCode;

        /**
         * vertex
         */
        std::vector<VulkanVertex> mVertices;

        /**
         * uniform
         */
        std::vector<VulkanDescriptorSet> mDescriptorSets;

        /**
         * push constant
         */
        std::vector<VulkanPushConstant> mPushConstants;

    public:

        VulkanShaderConfigure(VulkanGraphicsEngineBuilder &builder);

        ~VulkanShaderConfigure();

        /**
         *
         * shader coce
         *
         */
        VulkanShaderConfigure &vertexShaderCode(std::vector<char> &&code);

        VulkanShaderConfigure &fragmentShaderCode(std::vector<char> &&code);


        /**
         *
         * vertex
         *
         */
        VulkanShaderConfigure &vertex(const std::function<void(VulkanVertexConfigure &)> &configure);

        VulkanShaderConfigure &addVertex(const VulkanVertex &vertex);


        /**
         *
         * DescriptorSet
         *
         */
        VulkanShaderConfigure &addUniformSet(const VulkanDescriptorSet &uniformSet);

        VulkanShaderConfigure &uniformSet(const std::function<void(VulkanDescriptorSetConfigure &)> &configure);


        /**
         * push constant
         */
        VulkanShaderConfigure &addPushConstant(uint32_t size, uint32_t offset, vk::ShaderStageFlagBits stageFlagBits);

        /**
         *
         * build
         *
         */
        VulkanGraphicsEngineBuilder &build();
    };

} // engine
