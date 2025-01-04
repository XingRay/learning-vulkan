//
// Created by leixing on 2024/5/14.
//

#include "FileUtil.h"

std::vector<char> FileUtil::readFile(const std::string &path) {
    std::ifstream fileStream(path, std::ios::ate | std::ios::binary);
    if (!fileStream.is_open()) {
        throw std::runtime_error("failed to open file");
    }

    size_t fileSize = fileStream.tellg();
    std::vector<char> buffer(fileSize);

    fileStream.seekg(0);
    fileStream.read(buffer.data(), (std::streamsize) fileSize);
    fileStream.close();
    return buffer;
}


std::vector<uint32_t> FileUtil::convertToUint32Vector(const std::vector<char> &charVector) {
    // 确保输入数据按 4 字节对齐
    if (charVector.size() % sizeof(uint32_t) != 0) {
        throw std::runtime_error("Input vector size is not aligned to 4 bytes");
    }

    // 计算转换后的元素个数
    size_t numElements = charVector.size() / sizeof(uint32_t);

    // 创建目标 vector 并分配空间
    std::vector<uint32_t> uint32Vector(numElements);

    // 复制数据
    std::memcpy(uint32Vector.data(), charVector.data(), charVector.size());

    return uint32Vector;
}
