#pragma once
#include <cstdint>

#ifdef __linux__ 
    #include <time.h>
#elif _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#endif

namespace Time {
    static void sleep_ms(uint32_t milliseconds) {
#ifdef __linux__ 
        struct timespec sleep_time = { 0, 0 };
        sleep_time.tv_nsec = milliseconds * 1000 * 1000;
        nanosleep(&sleep_time, NULL);
#elif _WIN32
        Sleep(milliseconds);
#endif
    }
}