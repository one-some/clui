#pragma once
#include <cstdint>

namespace Ray {
#include "raylib.h"
}

struct Vector2 {
    int32_t x;
    int32_t y;

    Vector2 operator+(const Vector2& that) {
        return {x + that.x, y + that.y};
    }

    Vector2 operator-(const Vector2& that) {
        return {x - that.x, y - that.y};
    }

    Vector2 operator/(const int32_t that) {
        return {x / that, y / that};
    }

    bool in_rectangle(Vector2 pos, Vector2 size) {
        if (this->y - pos.y < 0) return false;
        if (this->y - pos.y > size.y) return false;

        if (this->x - pos.x < 0) return false;
        if (this->x - pos.x > size.x) return false;

        return true;
    }

    const Ray::Vector2 to_ray() {
        return { (float)x, (float)y };
    }

    static Vector2 from_ray(Ray::Vector2 vec) {
        return { (int32_t)vec.x, (int32_t)vec.y };
    }

    const static Ray::Rectangle to_ray_rect(Vector2 pos, Vector2 size) {
        return { (float)pos.x, (float)pos.y, (float)size.x, (float)size.y };
    }
};
