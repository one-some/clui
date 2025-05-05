#pragma once
#include "UI/Container/Container.h"
#include "Claire/String.h"
#include "Claire/File.h"
#include "cpp.h"
#include "fontglobal.h"

class TextEdit : public Container {
    public:
        String text;
        //String("Hello Folks\nFus Ro Dah.\nClaire speaking.\nLet's code something fantastic....!!!\nLove u jamie");
        RayLib::Font font = Font::the();

        Vector2 caret_position_px = {0, 0};
        int32_t caret_blink_timer = 0;
        CPPParser parser;

        int32_t font_size_px = 16;

        virtual void draw_self();
        void draw_text();
        void draw_text_plain_jane();

        static const int32_t CARET_BLINK_DURATION = 530 / 8;

        TextEdit(char* path) {
            text = File(path).read();
            parser = CPPParser(text);
            parser.parse();
        }
    
    private:
        void on_click() override;
};