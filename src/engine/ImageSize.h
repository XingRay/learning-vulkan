//
// Created by leixing on 2025/1/14.
//

#pragma once

#include <cstdint>

namespace engine {

    class ImageSize {
    public:
        uint32_t width;
        uint32_t height;
        uint32_t channels;

        ImageSize(uint32_t width, uint32_t height, uint32_t channels)
                : width(width), height(height), channels(channels) {
        }
    };

} // engine
