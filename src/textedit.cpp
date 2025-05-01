#include <cstring>
#include <stdio.h>
#include "color.h"
#include "textedit.h"
#include "string.h"
#include <vector>

void TextEdit::draw_self() {
    if (!font) return;

    caret_blink_timer++;

    bool caret_visible = true;

    if (caret_blink_timer > CARET_BLINK_DURATION) {
        caret_blink_timer = 0;
    } else if (caret_blink_timer > CARET_BLINK_DURATION / 2) {
        caret_visible = false;
    }

    Vector2 pos = position->get_global();
    Ray::DrawRectangle(
        pos.x,
        pos.y,
        1,
        size->get().y,
        Ray::ColorAlpha(Colors::FG, 0.2)
    );

    draw_text();

    if (caret_visible) {
        Ray::DrawRectangle(
            pos.x + caret_position_px.x + 1,
            pos.y + caret_position_px.y,
            2,
            font_size_px,
            Colors::FG
        );
    }
}

void TextEdit::draw_text_plain_jane() {
    Vector2 pos = position->get_global();
    std::vector<String> lines = text.split('\n');

    for (size_t i=0; i<lines.size();i++) {
        Ray::DrawTextEx(
            *font,
            lines[i].as_c(),
            { (float)pos.x + 4, (float)pos.y + (font_size_px * i) },
            (float)font_size_px,
            0,
            Colors::FG
        );
    }
}

void TextEdit::draw_text() {
    Vector2 pointer = position->get_global();

    for (auto node : parser.tokens) {
        if (node.type == TokenType::NEWLINE) {
            pointer.y += font_size_px;
            pointer.x = 0;
            continue;
        }

        Ray::Color color = Colors::FG;

        switch (node.type) {
            case TokenType::SYMBOL:

                switch (node.text.hash()) {
                    case String::hash("int32_t"):
                    case String::hash("switch"):
                    case String::hash("case"):
                    case String::hash("if"):
                    case String::hash("else"):
                    case String::hash("class"):
                    case String::hash("public"):
                    case String::hash("static"):
                    case String::hash("private"):
                    case String::hash("const"):
                    case String::hash("void"):
                        color = Ray::BLUE;
                        break;
                    case String::hash("nullptr"):
                        color = Ray::PURPLE;
                        break;
                }

                break;
            case TokenType::NUMBER:
                color = Ray::GREEN;
                break;
            case TokenType::COLON:
            case TokenType::SEMICOLON:
            case TokenType::DOT:
                color = Ray::GRAY;
                break;
            case TokenType::ASTERISK:
            case TokenType::EQUALS:
                color = Ray::WHITE;
                break;
            case TokenType::OPEN_PAREN:
            case TokenType::CLOSE_PAREN:
            case TokenType::OPEN_BRACE:
            case TokenType::CLOSE_BRACE:
                color = Ray::SKYBLUE;
            default:
                break;
        }

        if (node.commented) {
            color = Ray::DARKGREEN;
        }


        Ray::DrawTextEx(
            *font,
            node.text.as_c(),
            { (float)pointer.x + 4, (float)pointer.y},
            (float)font_size_px,
            0,
            color
        );

        pointer.x += Ray::MeasureTextEx(*font, node.text.as_c(), font_size_px, 0).x;
    }
}

void TextEdit::on_click() {
    Vector2 mouse_pos = Vector2::from_ray(Ray::GetMousePosition());
    mouse_pos = mouse_pos - position->get_global();

    size_t line_number = mouse_pos.y / font_size_px;
    std::vector<String> lines = text.split('\n');
    if (line_number > lines.size() - 1) line_number = lines.size() - 1;

    String line = lines[line_number];
    int32_t x_left = mouse_pos.x;

    size_t line_length = strlen(line.as_c());
    for (size_t i=1; i < line_length; i++) {
        int32_t width = Ray::MeasureTextEx(*font, line.first_n(i).as_c(), font_size_px, 0).x;

        if (width < x_left && i != line_length - 1) continue;

        caret_position_px.x = width;
        caret_position_px.y = line_number * font_size_px;
        caret_blink_timer = 0;

        printf("(%i, %i)\n", caret_position_px.x, caret_position_px.y);
        printf("%c\n", line.as_c()[i - 1]);
        break;
    }
}