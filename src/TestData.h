//
// Created by leixing on 2025/1/4.
//

#ifndef LEARNING_VULKAN_TESTDATA_H
#define LEARNING_VULKAN_TESTDATA_H

#include "glm.h"

struct Vertex {
    // 位置 3d
    glm::vec3 pos;

    // 颜色
    glm::vec3 color;

    // 纹理坐标
    glm::vec2 texCoord;

    static vk::VertexInputBindingDescription getBindingDescription() {
        vk::VertexInputBindingDescription description;

        // 绑定描述
        description.binding = 0;

        // 步幅，表示每个顶点数据的字节数
        description.stride = sizeof(Vertex);

        // 输入速率，表示每个顶点数据的读取频率
        description.inputRate = vk::VertexInputRate::eVertex;

        return description;
    }

    static std::array<vk::VertexInputAttributeDescription, 3> getAttributeDescriptions() {
        std::array<vk::VertexInputAttributeDescription, 3> attributeDescriptions{};

        // 位置
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = vk::Format::eR32G32B32Sfloat;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);

        // 颜色
        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = vk::Format::eR32G32B32Sfloat;
        attributeDescriptions[1].offset = offsetof(Vertex, color);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = vk::Format::eR32G32Sfloat;
        attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

        return attributeDescriptions;
    }

    bool operator==(const Vertex &other) const {
        return pos == other.pos && color == other.color && texCoord == other.texCoord;
    }
};

namespace std {
    template<>
    struct hash<Vertex> {
        size_t operator()(Vertex const &vertex) const {
            return ((hash<glm::vec3>()(vertex.pos) ^
                     (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^
                   (hash<glm::vec2>()(vertex.texCoord) << 1);
        }
    };
}


struct UniformBufferObject {
    // alignas(16) 显示指定字段的对齐方式，
    // 标量必须按N对齐（对于32位浮点数，N = 4字节）。
    // vec2必须按2N对齐（= 8字节）。
    // vec3或vec4必须按4N对齐（= 16字节）。
    // 嵌套结构必须按其成员的基准对齐向上取整至16的倍数对齐。
    // mat4矩阵必须与vec4具有相同的对齐方式。
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
};

#endif //LEARNING_VULKAN_TESTDATA_H
