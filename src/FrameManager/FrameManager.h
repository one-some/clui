#pragma once

#include <vector>
#include <functional>

class FrameManager {
    public:
        static void queue_operation(std::function<void()> operation);
        static void run_queued_operations();

    private:
        static std::vector<std::function<void()>> queued_operations;
};