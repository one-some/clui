#pragma once

#include <string>
#include <cstdarg>
#include <stdlib.h>
#include <stdio.h>

#ifdef __linux__ 
    #include <stacktrace>
#endif

#define ASSERT(condition, format, ...) \
    _ASSERT(condition, __FILE__, __func__, __LINE__, format, ##__VA_ARGS__)

#define ASSERT_NOT_REACHED(format, ...) \
    _ASSERT_NOT_REACHED(__FILE__, __func__, __LINE__, format, ##__VA_ARGS__)

constexpr size_t ASSERT_BUF_SIZE = 2048;

inline void _dump_stack() {
#ifdef __linux__ 
    printf("%s\n", std::to_string(std::stacktrace::current()).c_str());
#elif _WIN32
    printf("( Stack trace ommitted on WIN32 builds. Sorry! )\n");
#endif
}

inline void _ASSERT(
    bool condition,
    const char* file,
    const char* func,
    int line,
    const char* format,
    ...
) {
    if (condition) return;

    // Or so
    char message[ASSERT_BUF_SIZE];
    va_list args;
    va_start(args, format);
    vsnprintf(message, sizeof(message), format, args);
    va_end(args);

    printf("[%s -> %s:%d] Assertion failed! :: %s\n", file, func, line, message);
    _dump_stack();
    exit(1);
}

[[noreturn]] inline void _ASSERT_NOT_REACHED(
    const char* file,
    const char* func,
    int line,
    const char* format,
    ...
) {
    char message[ASSERT_BUF_SIZE];
    va_list args;
    va_start(args, format);
    vsnprintf(message, sizeof(message), format, args);
    va_end(args);

    printf("[%s -> %s:%d] ASSERT_NOT_REACHED reached! :: %s\n", file, func, line, message);
    _dump_stack();
    exit(1);
}