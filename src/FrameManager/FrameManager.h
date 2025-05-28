#pragma once

#include <vector>
#include <functional>
#include "Dep/raylib.h"
#include "vector2.h"
#include "UI/Container/Container.h"

struct DebugLine {
    int32_t pos = 0;
    bool is_vertical = true;
};


class FrameManager {
    public:
        static Container* root_container;
        static std::vector<Vector2> debug_points;
        static std::vector<DebugLine> debug_lines;

        static void queue_operation(std::function<void()> operation);
        static void run_queued_operations();

        // The idea with this: Anyone should be able to say Oh! Oh! Make the
        // cursor this! Without having to say Okay not anymore!.... because if
        // they do that then someone else is gonna be like HEY why did you stop
        // making MY cursoir!>!?!?!?!
        static void set_frame_cursor(RayLib::MouseCursor cursor);
        static RayLib::MouseCursor get_frame_cursor();

        static void add_debug_point(Vector2 pos);
        static void add_debug_line(DebugLine line);
        static void draw_debug_points();

    private:
        static std::vector<std::function<void()>> queued_operations;
        static RayLib::MouseCursor frame_cursor;
};