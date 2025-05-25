#include "FrameManager/FrameManager.h"
#include <stdio.h>

std::vector<std::function<void()>> FrameManager::queued_operations;

void FrameManager::queue_operation(std::function<void()> operation) {
    queued_operations.push_back(operation);
}

void FrameManager::run_queued_operations() {
    for (auto& operation : queued_operations) {
        operation();
    }

    queued_operations.clear();
}