#pragma once

#include <cstdarg>
#include <stdlib.h>
#include <stdio.h>
#include <stacktrace>

#define ASSERT(condition, format, ...) \
    _ASSERT(condition, __FILE__, __func__, __LINE__, format, ##__VA_ARGS__)

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
    char message[2048];
    va_list args;
    va_start(args, format);
    vsnprintf(message, sizeof(message), format, args);
    va_end(args);

    printf("[%s -> %s:%d] Assertion failed! :: %s\n", file, func, line, message);
    printf("%s\n", std::to_string(std::stacktrace::current()).c_str());
    exit(1);
}

[[noreturn]] inline void ASSERT_NOT_REACHED(const char* message) {
    printf("Assertion reached! :: %s\n", message);
    exit(1);
}