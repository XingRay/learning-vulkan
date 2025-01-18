//
// Created by one on 2024/12/10.
//

#pragma once

// Android log function wrappers
static const char *TAG = "Vulkan_Demo";

#ifdef WIN32

#include <cstdio>
#include <ctime>
#include <string>

// 时间戳获取函数
inline std::string currentDateTime() {
    char buffer[100];
    time_t now = time(nullptr);
    struct tm tstruct;
    localtime_s(&tstruct, &now); // 安全的时间格式化函数
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %X", &tstruct);
    return std::string(buffer);
}

// Windows 日志宏定义
#define LOG_V(...) (printf("[VERBOSE] [%s] [%s] ", currentDateTime().c_str(), TAG), printf(__VA_ARGS__), printf("\n"))
#define LOG_D(...) (printf("[DEBUG]   [%s] [%s] ", currentDateTime().c_str(), TAG), printf(__VA_ARGS__), printf("\n"))
#define LOG_I(...) (printf("[INFO]    [%s] [%s] ", currentDateTime().c_str(), TAG), printf(__VA_ARGS__), printf("\n"))
#define LOG_W(...) (printf("[WARN]    [%s] [%s] ", currentDateTime().c_str(), TAG), printf(__VA_ARGS__), printf("\n"))
#define LOG_E(...) (printf("[ERROR]   [%s] [%s] ", currentDateTime().c_str(), TAG), printf(__VA_ARGS__), printf("\n"))

#endif


#ifdef __ANDROID__
#include <android/log.h>
#define LOG_V(...) ((void)__android_log_print(ANDROID_LOG_VERBOSE, TAG, __VA_ARGS__))
#define LOG_D(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__))
#define LOG_I(...) ((void)__android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__))
#define LOG_W(...) ((void)__android_log_print(ANDROID_LOG_WARN, TAG, __VA_ARGS__))
#define LOG_E(...) ((void)__android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__))
#endif