#include <cstring>
#include <stdio.h>
#include "textedit.h"
#include "string.h"
#include <vector>

void TextEdit::draw_self() {
    if (!font) return;

    Vector2 pos = position->get_global();
    Ray::DrawRectangle(
        pos.x,
        pos.y,
        3,
        size->get().y,
        Ray::ColorAlpha(Ray::BLACK, 0.4)
    );

    std::vector<String> lines = text.split('\n');

    for (size_t i=0; i<lines.size();i++) {
        Ray::DrawTextEx(
            *font,
            lines[i].as_c(),
            { (float)pos.x, (float)pos.y + (font->baseSize * i) },
            (float)font->baseSize,
            0,
            Ray::BLACK
        );
    }
}

void TextEdit::on_click() {
    Vector2 mouse_pos = Vector2::from_ray(Ray::GetMousePosition());
    mouse_pos = mouse_pos - position->get_global();

    size_t line_number = mouse_pos.y / font->baseSize;
    std::vector<String> lines = text.split('\n');
    if (line_number > lines.size() - 1) line_number = lines.size() - 1;

    String line = lines[line_number];
    int32_t x_left = mouse_pos.x;

    for (size_t i=1; i < strlen(line.as_c()); i++) {
        int32_t width = Ray::MeasureTextEx(*font, line.first_n(i).as_c(), font->baseSize, 0).x;
        printf("%i\n", width);

        if (width < x_left) continue;

        printf("%c\n", line.as_c()[i - 1]);
        break;
    }
}