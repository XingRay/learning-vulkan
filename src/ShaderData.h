//
// Created by leixing on 2024/12/29.
//

#ifndef VULKANDEMO_SHADERDATA_H
#define VULKANDEMO_SHADERDATA_H

#include "glm.h"
#include "vulkan/vulkan.hpp"
#include "Log.h"

namespace app {
    struct Vertex {
        glm::vec3 position; // 对应顶点位置属性 inPosition

        // 返回顶点输入绑定和属性描述，供 Vulkan Pipeline 使用
        static vk::VertexInputBindingDescription getBindingDescription() {

            size_t stride = sizeof(Vertex);
            LOG_D("VertexInputBindingDescription, stride: %lld", stride);

            vk::VertexInputBindingDescription bindingDescription{};
            bindingDescription.binding = 0;
            bindingDescription.stride = stride;
            bindingDescription.inputRate = vk::VertexInputRate::eVertex;
            return bindingDescription;
        }

        static vk::VertexInputAttributeDescription getAttributeDescription() {

            size_t offset = offsetof(Vertex, position);
            LOG_D("VertexInputAttributeDescription, offset: %lld", offset);

            vk::VertexInputAttributeDescription attributeDescription{};
            attributeDescription.binding = 0;
            attributeDescription.location = 0;
            attributeDescription.format = vk::Format::eR32G32B32Sfloat;
            attributeDescription.offset = offset;

            return attributeDescription;
        }
    };

    struct TransformUniformBufferObject {
        glm::mat4 model; // 模型矩阵
        glm::mat4 view;  // 视图矩阵
        glm::mat4 proj;  // 投影矩阵
    };

    struct ColorUniformBufferObject {
        alignas(16) glm::vec3 color; // 颜色数据
    };

}

#endif //VULKANDEMO_SHADERDATA_H
