//
// Created by leixing on 2024/5/14.
//

#pragma once

#include <vector>
#include <string>
#include <fstream>

class FileUtil {
public:
    static std::vector<char> readFile(const std::string &path);

    static std::vector<uint32_t> convertToUint32Vector(const std::vector<char>& charVector);
};
