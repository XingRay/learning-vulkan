//
// Created by leixing on 2024/5/12.
//

#include "QueueFamilyIndices.h"

bool QueueFamilyIndices::isComplete() {
    return queueFamily.has_value();
}
