#pragma once

#include <vector>
#include <functional>
#include "Dep/raylib.h"

class FrameManager {
    public:
        static void queue_operation(std::function<void()> operation);
        static void run_queued_operations();

        // The idea with this: Anyone should be able to say Oh! Oh! Make the
        // cursor this! Without having to say Okay not anymore!.... because if
        // they do that then someone else is gonna be like HEY why did you stop
        // making MY cursoir!>!?!?!?!
        static void set_frame_cursor(RayLib::MouseCursor cursor);
        static RayLib::MouseCursor get_frame_cursor();

    private:
        static std::vector<std::function<void()>> queued_operations;
        static RayLib::MouseCursor frame_cursor;
};