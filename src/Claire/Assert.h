#pragma once

#include <stdlib.h>
#include <stdio.h>

inline void ASSERT(bool condition, const char* message) {
    if (condition) return;

    printf("Assertion failed! :: %s", message);
    exit(1);
}