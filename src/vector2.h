#pragma once
#include <cstdint>

struct Vector2 {
    int32_t x;
    int32_t y;

    Vector2 operator+(const Vector2& that) {
        return {x + that.x, y + that.y};
    }

    bool in_rectangle(Vector2 pos, Vector2 size) {
        if (this->y - pos.y < 0) return false;
        if (this->y - pos.y > size.y) return false;

        if (this->x - pos.x < 0) return false;
        if (this->x - pos.x > size.x) return false;

        return true;
    }
};
