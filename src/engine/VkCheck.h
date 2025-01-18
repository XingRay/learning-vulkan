//
// Created by one on 2024/12/10.
//

#pragma once

// Vulkan call wrapper
#define CALL_VK(func)                                                 \
  if (VK_SUCCESS != (func)) {                                         \
    LOG_E("Vulkan error. File[%s], line[%d]", __FILE__, __LINE__);     \
    assert(false);                                                    \
  }
