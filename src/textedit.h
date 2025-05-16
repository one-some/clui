// Hello from CLAIRE

#pragma once
#include "UI/Container/Container.h"
#include "Claire/String.h"
#include "Claire/File.h"
#include "cpp.h"
#include "fontglobal.h"

class TextEdit : public Container {
    public:
        String text;
        File file;

        //String("Hello Folks\nFus Ro Dah.\nClaire speaking.\nLet's code something fantastic....!!!\nLove u jamie");
        RayLib::Font font = Font::the();

        Vector2 caret_position_px = {0, 0};
        size_t caret_index = 0;
        int32_t caret_blink_timer = 0;
        CPPParser parser;
        const char* path;

        int32_t font_size_px = 16;

        virtual void draw_self();
        void move_caret(Vector2 delta);
        void save_to_file();
        static size_t str_index_from_vec2(const char* text, Vector2 vec);

        static const int32_t CARET_BLINK_DURATION = 530 / 8;

        TextEdit(const char* path): file(path) {
            printf("Hello from crazy world...\n");
            this->path = path;
            text = file.read();
            parser = CPPParser(&text);
            parser.parse();
        }
    
    private:
        void on_click() override;
        void on_input() override;
        void draw_text();
        void draw_text_plain_jane();

        Vector2 survey_position(size_t index);
};