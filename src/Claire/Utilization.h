#pragma once

#include "Claire/File.h"

double ram_mb() {
    auto text = File("/proc/self/status").read();

    for (auto& line : text.split("\n")) {
        if (!line.starts_with("VmRSS:")) continue;

        for (auto& chunk : line.split(" ")) {
            if (!chunk.is_number()) continue;
            return chunk.to_int() / 1000.0;
        }
    }

    ASSERT_NOT_REACHED("Can't get RAM lol");
}