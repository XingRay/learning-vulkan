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
#define LOGV(...) (printf("[VERBOSE] [%s] [%s] ", currentDateTime().c_str(), TAG), printf(__VA_ARGS__), printf("\n"))
#define LOGD(...) (printf("[DEBUG]   [%s] [%s] ", currentDateTime().c_str(), TAG), printf(__VA_ARGS__), printf("\n"))
#define LOGI(...) (printf("[INFO]    [%s] [%s] ", currentDateTime().c_str(), TAG), printf(__VA_ARGS__), printf("\n"))
#define LOGW(...) (printf("[WARN]    [%s] [%s] ", currentDateTime().c_str(), TAG), printf(__VA_ARGS__), printf("\n"))
#define LOGE(...) (printf("[ERROR]   [%s] [%s] ", currentDateTime().c_str(), TAG), printf(__VA_ARGS__), printf("\n"))

#endif


#ifdef __ANDROID__
#include <android/log.h>
#define LOGV(...) ((void)__android_log_print(ANDROID_LOG_VERBOSE, TAG, __VA_ARGS__))
#define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__))
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, TAG, __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__))
#endif


#define LOG_V(...) LOGV(__VA_ARGS__)
#define LOG_D(...) LOGD(__VA_ARGS__)
#define LOG_I(...) LOGI(__VA_ARGS__)
#define LOG_W(...) LOGW(__VA_ARGS__)
#define LOG_E(...) LOGE(__VA_ARGS__)
