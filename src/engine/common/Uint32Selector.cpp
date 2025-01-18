//
// Created by leixing on 2025/1/11.
//

#include "Uint32Selector.h"

#include <stdexcept>

namespace common {
    FixUint32Selector::FixUint32Selector(uint32_t value) : mFixValue(value) {
    }

    FixUint32Selector::~FixUint32Selector() = default;

    uint32_t FixUint32Selector::select(const std::vector<uint32_t> &candidates) const {
        bool found = false;
        for (int i = 0; i < candidates.size(); i++) {
            if (candidates[i] == mFixValue) {
                found = true;
            }
        }
        if (!found) {
            throw std::runtime_error("value not found");
        }
        return mFixValue;
    }

    MaxUint32Selector::MaxUint32Selector() : mMaxLimit(std::numeric_limits<uint32_t>::max()) {
    }

    MaxUint32Selector::MaxUint32Selector(uint32_t maxLimit) : mMaxLimit(maxLimit) {
    }

    MaxUint32Selector::~MaxUint32Selector() = default;


    uint32_t MaxUint32Selector::select(const std::vector<uint32_t> &candidates) const {
        bool found = false;
        uint32_t value = 0;
        for (int i = 0; i < candidates.size(); i++) {
            if (candidates[i] > value && candidates[i] <= mMaxLimit) {
                value = candidates[i];
                found = true;
            }
        }
        if (!found) {
            throw std::runtime_error("value not found");
        }
        return value;
    }


    LambdaUint32Selector::LambdaUint32Selector(std::function<uint32_t(const std::vector<uint32_t> &)> selector)
        : mSelector(std::move(selector)) {
    }

    LambdaUint32Selector::~LambdaUint32Selector() = default;

    [[nodiscard]]
    uint32_t LambdaUint32Selector::select(const std::vector<uint32_t> &candidates) const {
        return mSelector(candidates);
    }
};

// namespace common
