#pragma once
#include "container.h"
#include "string.h"
#include "file.h"
#include "cpp.h"
#include "fontglobal.h"

class TextEdit : public Container {
    public:
        String text = File("src/textedit.h").read();
        //String("Hello Folks\nFus Ro Dah.\nClaire speaking.\nLet's code something fantastic....!!!\nLove u jamie");
        Ray::Font font = Font::the();

        Vector2 caret_position_px = {0, 0};
        int32_t caret_blink_timer = 0;
        CPPParser parser = CPPParser(File("src/textedit.h").read());

        int32_t font_size_px = 16;

        virtual void draw_self();
        void draw_text();
        void draw_text_plain_jane();

        static const int32_t CARET_BLINK_DURATION = 530 / 8;

        TextEdit() {
            parser.parse();
        }
    
    private:
        void on_click() override;
};