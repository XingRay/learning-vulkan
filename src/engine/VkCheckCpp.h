//
// Created by leixing on 2024/12/17.
//

#ifndef VULKANDEMO_VKCHECKCPP_H
#define VULKANDEMO_VKCHECKCPP_H


#include <stdexcept>
#include <vulkan/vulkan.hpp>
#include "Log.h"

// C++ 风格的 Vulkan 调用包装函数
inline void CallVulkan(const vk::Result& result, const char* file, int line) {
    if (result != vk::Result::eSuccess) {
        LOG_E("Vulkan error. File[ %s ], line[ %d ]", file, line);
        throw std::runtime_error("Vulkan API call failed");
    }
}

// 宏用于自动传递文件名和行号
#define CALL_VK_CPP(func) CallVulkan((func), __FILE__, __LINE__)


#endif //VULKANDEMO_VKCHECKCPP_H
