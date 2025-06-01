#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <stacktrace>

#define ASSERT(condition, message) _ASSERT(condition, message, __FILE__, __func__, __LINE__)

inline void _ASSERT(
    bool condition,
    const char* message,
    const char* file,
    const char* func,
    int line
) {
    if (condition) return;

    printf("[%s -> %s:%d] Assertion failed! :: %s\n", file, func, line, message);
    printf("%s\n", std::to_string(std::stacktrace::current()).c_str());
    exit(1);
}

[[noreturn]] inline void ASSERT_NOT_REACHED(const char* message) {
    printf("Assertion reached! :: %s\n", message);
    exit(1);
}