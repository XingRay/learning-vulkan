//
// Created by leixing on 2025/1/9.
//

#pragma once

#include <string>
#include <vector>

namespace common {

    class StringUtil {
    public:
        static std::vector<uint32_t> parseVersion(const std::string &version);

        static bool isContains(std::vector<std::string> src, std::vector<std::string> target);

        static std::vector<const char *> toStringPtrArray(const std::vector<std::string> &strings);
    };

} // common
