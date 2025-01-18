//
// Created by leixing on 2025/1/5.
//

#pragma once

#include <vulkan/vulkan.hpp>
#include <stdexcept>
#include <unordered_map>

enum class ShaderFormat {
    Bool,       // bool
    Float,      // float
    Int,        // int
    Vec2,       // vec2
    Vec3,       // vec3
    Vec4,       // vec4
    Mat2,       // mat2
    Mat3,       // mat3
    Mat4        // mat4
};

class ShaderFormatHelper {
public:
    static vk::Format toVkFormat(ShaderFormat format) {
        static const std::unordered_map<ShaderFormat, vk::Format> shaderFormatMap = {
                {ShaderFormat::Bool, vk::Format::eR8Uint},
                {ShaderFormat::Float, vk::Format::eR32Sfloat},
                {ShaderFormat::Int, vk::Format::eR32Sint},
                {ShaderFormat::Vec2, vk::Format::eR32G32Sfloat},
                {ShaderFormat::Vec3, vk::Format::eR32G32B32Sfloat},
                {ShaderFormat::Vec4, vk::Format::eR32G32B32A32Sfloat},
                {ShaderFormat::Mat2, vk::Format::eR32G32Sfloat},       // Matrices require custom handling
                {ShaderFormat::Mat3, vk::Format::eR32G32B32Sfloat},    // Custom row-wise matrix format
                {ShaderFormat::Mat4, vk::Format::eR32G32B32A32Sfloat}  // Custom row-wise matrix format
        };

        auto it = shaderFormatMap.find(format);
        if (it != shaderFormatMap.end()) {
            return it->second;
        }
        throw std::invalid_argument("Unsupported ShaderFormat");
    }
};