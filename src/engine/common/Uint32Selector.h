//
// Created by leixing on 2025/1/11.
//

#pragma once

#include <vector>
#include <functional>

#include "engine/common/Selector.h"


namespace common {

    class FixUint32Selector : public ValueSelector<uint32_t> {
    private:
        uint32_t mFixValue;

    public:
        FixUint32Selector(uint32_t value);

        ~FixUint32Selector() override;

        [[nodiscard]]
        uint32_t select(const std::vector<uint32_t> &candidates) const override;
    };

    class MaxUint32Selector : public ValueSelector<uint32_t> {
    private:
        uint32_t mMaxLimit;

    public:
        MaxUint32Selector();

        MaxUint32Selector(uint32_t maxLimit);

        ~MaxUint32Selector() override;

        [[nodiscard]]
        uint32_t select(const std::vector<uint32_t> &candidates) const override;
    };

    class LambdaUint32Selector : public ValueSelector<uint32_t> {
    private:
        std::function<uint32_t(const std::vector<uint32_t> &)> mSelector;

    public:
        explicit LambdaUint32Selector(std::function<uint32_t(const std::vector<uint32_t> &)> selector);

        ~LambdaUint32Selector();

        [[nodiscard]]
        uint32_t select(const std::vector<uint32_t> &candidates) const override;
    };

} // common
