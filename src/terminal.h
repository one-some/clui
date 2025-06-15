#pragma once
#include "UI/Container/Container.h"
#include "Claire/String.h"
#include "fontglobal.h"

class Terminal : public Container {
    public:
        static const int32_t CARET_BLINK_DURATION = 530 / 8;

        String text;
        int master_pty_fd = -1;

        RayLib::Font font = Font::the();
        int32_t font_size_px = 16;
        int32_t caret_blink_timer = 0;
        uint32_t caret_pos = 0;

        Terminal() {
            allow_scroll = true;
            init_terminal();
        }

        virtual void draw_self() override;
        void draw_text();
        void init_terminal();
    
    private:
        void on_input() override;
        void sync_pty();
};