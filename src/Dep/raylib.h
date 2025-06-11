#pragma once

#include <cstdarg>

namespace RayLib {
    #include "Dep/raylib_raw.h"

    static MouseCursor current_mouse_cursor = MouseCursor::MOUSE_CURSOR_DEFAULT;

    inline void AwesomeSetMouseCursor(MouseCursor cursor) {
        if (cursor == current_mouse_cursor) return;
        current_mouse_cursor = cursor;
        SetMouseCursor(cursor);
    }

    inline bool IsKeyTyped(int key) {
        return IsKeyPressed(key) || IsKeyPressedRepeat(key);
    }

    class SmartImage {
    public:
        Image image;

        SmartImage(const char* path) : image(LoadImage(path)) { }

        ~SmartImage() {
            UnloadImage(image);
        }
    };
}