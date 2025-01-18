//
// Created by leixing on 2025/1/11.
//

#pragma once

#include <vector>

namespace common {

    template<typename T>
    class ValueSelector {
    public:
        virtual ~ValueSelector() = default;

        [[nodiscard]]
        virtual T select(const std::vector<T> &candidates) const = 0;
    };

    template<typename T>
    class ListSelector {
    public:
        virtual ~ListSelector() = default;

        [[nodiscard]]
        virtual std::vector<T> select(const std::vector<T> &candidates) const = 0;
    };
}
