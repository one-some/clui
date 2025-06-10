#pragma once
#include <cstdint>
#include "Dep/raylib.h"

struct Vector2 {
    int32_t x = 0;
    int32_t y = 0;

    Vector2 operator+(const Vector2& that) {
        return {x + that.x, y + that.y};
    }

    Vector2 operator-(const Vector2& that) {
        return {x - that.x, y - that.y};
    }

    Vector2 operator*(const double that) {
        return {(int32_t)(x * that), (int32_t)(y * that)};
    }

    Vector2 operator/(const double that) {
        return {(int32_t)(x / that), (int32_t)(y / that)};
    }

    operator bool() const {
        return x || y;
    }

    bool in_rectangle(Vector2 pos, Vector2 size) {
        if (this->y - pos.y < 0) return false;
        if (this->y - pos.y > size.y) return false;

        if (this->x - pos.x < 0) return false;
        if (this->x - pos.x > size.x) return false;

        return true;
    }

    void graft(Vector2 that) {
        x = that.x;
        y = that.y;
    }

    const RayLib::Vector2 to_ray() {
        return { (float)x, (float)y };
    }

    static Vector2 zero() {
        return { 0, 0 };
    }

    static Vector2 from_ray(RayLib::Vector2 vec) {
        return { (int32_t)vec.x, (int32_t)vec.y };
    }

    const static RayLib::Rectangle to_ray_rect(Vector2 pos, Vector2 size) {
        return { (float)pos.x, (float)pos.y, (float)size.x, (float)size.y };
    }
};
