#include "FrameManager/FrameManager.h"
#include <stdio.h>

RayLib::MouseCursor FrameManager::frame_cursor = RayLib::MouseCursor::MOUSE_CURSOR_DEFAULT;
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

void FrameManager::set_frame_cursor(RayLib::MouseCursor cursor) { frame_cursor = cursor; }
RayLib::MouseCursor FrameManager::get_frame_cursor() { return frame_cursor; }