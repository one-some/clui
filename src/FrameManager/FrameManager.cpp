#include "FrameManager/FrameManager.h"
#include <stdio.h>

Container* FrameManager::root_container = nullptr;
RayLib::MouseCursor FrameManager::frame_cursor = RayLib::MouseCursor::MOUSE_CURSOR_DEFAULT;
std::vector<std::function<void()>> FrameManager::queued_operations;
std::vector<Vector2> FrameManager::debug_points;
std::vector<DebugLine> FrameManager::debug_lines;

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

void FrameManager::add_debug_point(Vector2 pos) {
    debug_points.push_back(pos);
}

void FrameManager::add_debug_line(DebugLine line) {
    debug_lines.push_back(line);
}

void FrameManager::draw_debug_points() {
    for (auto& point : debug_points) {
        RayLib::DrawRectangle(point.x, point.y - 5, 1, 11, RayLib::MAGENTA);
        RayLib::DrawRectangle(point.x - 5, point.y, 11, 1, RayLib::MAGENTA);
    }

    for (auto& line : debug_lines) {
        if (line.is_vertical) {
            RayLib::DrawRectangle(line.pos, 0, 1, root_container->size->get().y, RayLib::MAGENTA);
        } else {
            RayLib::DrawRectangle(0, line.pos, root_container->size->get().x, 1, RayLib::MAGENTA);
        }
    }

    // debug_points.clear();
}