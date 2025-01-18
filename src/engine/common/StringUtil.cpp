//
// Created by leixing on 2025/1/9.
//

#include "StringUtil.h"

#include <algorithm>
#include <sstream>

namespace common {

    std::vector<uint32_t> StringUtil::parseVersion(const std::string &version) {
        std::vector<uint32_t> numbers;
        std::istringstream iss(version);
        std::string token;

        // 按 '.' 分割字符串
        while (std::getline(iss, token, '.')) {
            try {
                // 将每个部分转换为整数
                uint32_t number = std::stoul(token);
                numbers.push_back(number);
            } catch (const std::exception &) {
                throw std::invalid_argument("Invalid version format: non-numeric value found");
            }
        }

        return numbers;
    }


    bool StringUtil::isContains(std::vector<std::string> src, std::vector<std::string> target) {
        // 使用 std::all_of 检查 target 中的每个元素是否都在 src 中
        return std::all_of(target.begin(), target.end(), [&src](const std::string &item) {
            // 使用 std::find 在 src 中查找当前元素
            return std::find(src.begin(), src.end(), item) != src.end();
        });
    }

    std::vector<const char*> StringUtil::toStringPtrArray(const std::vector<std::string> &strings) {
        std::vector<const char*> ptrArray;
        ptrArray.reserve(strings.size());  // 预分配空间
        for (const auto &str : strings) {
            ptrArray.push_back(str.c_str());  // 将 std::string 转换为 const char*
        }
        return ptrArray;
    }


} // common