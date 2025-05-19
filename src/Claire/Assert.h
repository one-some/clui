#pragma once

#include <stdlib.h>
#include <stdio.h>

inline void ASSERT(bool condition, const char* message) {
    if (condition) return;

    printf("Assertion failed! :: %s", message);
    exit(1);
}

inline void ASSERT_NOT_REACHED(const char* message) {
    printf("Assertion reached! :: %s", message);
    exit(1);
}