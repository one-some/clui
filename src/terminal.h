#pragma once
#include "UI/Container/Container.h"
#include "Claire/String.h"
#include "fontglobal.h"

class Terminal : public Container {
    public:
        String text;
        int master_pty_fd = -1;

        RayLib::Font font = Font::the();
        int32_t font_size_px = 16;

        virtual void draw_self();
        void draw_text();
        void init_terminal();

        static const int32_t CARET_BLINK_DURATION = 530 / 8;

        Terminal() {
            init_terminal();
        }
    
    private:
        void sync_pty();
        void handle_input();
};