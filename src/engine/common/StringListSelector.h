//
// Created by leixing on 2025/1/8.
//

#pragma once

#include <vector>
#include <functional>
#include <string>

#include "engine/common/Selector.h"

namespace common {

    class FixStringListSelector : public ListSelector<std::string> {
    private:
        std::vector<std::string> mSelected;

    public:
        explicit FixStringListSelector(const std::vector<std::string> &selected);

        ~FixStringListSelector() override;

        [[nodiscard]]
        std::vector<std::string> select(const std::vector<std::string> &candidate) const override;
    };

    class RequiredAndOptionalStringListSelector : public ListSelector<std::string> {
    private:
        std::vector<std::string> mRequired;
        std::vector<std::string> mOptional;
    public:
        explicit RequiredAndOptionalStringListSelector(const std::vector<std::string> &required,
                                                       const std::vector<std::string> &optional = {});

        ~RequiredAndOptionalStringListSelector() override;

        [[nodiscard]]
        std::vector<std::string> select(const std::vector<std::string> &candidate) const override;
    };


    class LambdaStringListSelector : public ListSelector<std::string> {
    private:
        std::function<std::vector<std::string>(const std::vector<std::string> &)> mSelector;

    public:
        explicit LambdaStringListSelector(std::function<std::vector<std::string>(const std::vector<std::string> &)> selector);

        ~LambdaStringListSelector() override;

        [[nodiscard]]
        std::vector<std::string> select(const std::vector<std::string> &candidate) const override;
    };

} // common
