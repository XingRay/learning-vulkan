//
// Created by leixing on 2025/1/4.
//

#pragma once

#include <string>
#include <utility>

namespace test {
    class TestBase {
    public:
        virtual ~TestBase() = default;

        virtual void init() = 0;

        virtual bool isReady() = 0;

        virtual void drawFrame() = 0;

        virtual void cleanup() = 0;
    };
}
