//
// Created by leixing on 2025/1/8.
//

#include "StringListSelector.h"

#include <stdexcept>
#include <utility>
#include "engine/Log.h"

namespace common {
    FixStringListSelector::FixStringListSelector(const std::vector<std::string> &selected)
        : mSelected(selected) {
    }

    FixStringListSelector::~FixStringListSelector() {
        mSelected.clear();
    }

    std::vector<std::string> FixStringListSelector::select(const std::vector<std::string> &candidate) const {
        return mSelected;
    }


    RequiredAndOptionalStringListSelector::RequiredAndOptionalStringListSelector(const std::vector<std::string> &required,
                                                                                 const std::vector<std::string> &optional)
        : mRequired(required), mOptional(optional) {
    }

    RequiredAndOptionalStringListSelector::~RequiredAndOptionalStringListSelector() = default;

    std::vector<std::string> RequiredAndOptionalStringListSelector::select(const std::vector<std::string> &candidate) const {
        std::vector<std::string> result;

        // 1. 检查 mRequired 中的所有字符串是否都在 candidate 中
        for (const std::string &requiredStr: mRequired) {
            auto it = std::find(candidate.begin(), candidate.end(), requiredStr);
            if (it == candidate.end()) {
                // 如果某个必需的字符串不在 candidate 中，抛出异常
                throw std::runtime_error("Required string not found in candidate: " + requiredStr);
            }
            // 如果找到，则添加到返回值中
            result.push_back(requiredStr);
        }

        // 2. 检查 mOptional 中的所有字符串是否在 candidate 中
        for (const std::string &optionalStr: mOptional) {
            auto it = std::find(candidate.begin(), candidate.end(), optionalStr);
            if (it == candidate.end()) {
                // 如果某个可选的字符串不在 candidate 中，打日志提示
                LOG_W("Optional string not found in candidate: %s\n", optionalStr.c_str());
            } else {
                // 如果找到，则添加到返回值中
                result.push_back(optionalStr);
            }
        }

        return result;
    }


    LambdaStringListSelector::LambdaStringListSelector(std::function<std::vector<std::string>(const std::vector<std::string> &)> selector)
        : mSelector(std::move(selector)) {
    }

    LambdaStringListSelector::~LambdaStringListSelector() = default;

    std::vector<std::string> LambdaStringListSelector::select(const std::vector<std::string> &candidate) const {
        return mSelector(candidate);
    }
} // common
