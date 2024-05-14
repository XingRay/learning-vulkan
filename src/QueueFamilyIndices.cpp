//
// Created by leixing on 2024/5/12.
//

#include "QueueFamilyIndices.h"

bool QueueFamilyIndices::isComplete() {
    return graphicQueueFamily.has_value() && presentQueueFamily.has_value();
}
