#include <cstring>
#include <stdio.h>
#include "color.h"
#include "UI/TextEdit/TextEdit.h"
#include "FrameManager/FrameManager.h"
#include "Claire/String.h"
#include "Claire/Math.h"
#include <vector>

size_t TextEdit::str_index_from_vec2(const char* text, Vector2 vec) {
    for (size_t i = 0; text[i]; i++) {
        if (vec.y) {
            if (text[i] != '\n') continue;

            vec.y--;
            continue;
        }

        // FIXME: What do we do when either is inaccessable?
        vec.x--;

        if (!vec.x) return i + 1;
    }

    printf("WITS END.\n");
    return 0;
}

void TextEdit::move_caret(Vector2 delta) {
    char c = '\0';

    while ((c = text.as_c()[caret_index])) {
        if (delta.y) {
            if (caret_index == 0 && delta.y < 0) {
                delta.y = 0;
                continue;
            }

            // printf("Okay! Let's deal with delta y... Dy: %i, Sign: %i\n", delta.y, SIGN(delta.y));
            // printf("k.... derlta y is now %i\n", delta.y);
            caret_index += SIGN(delta.y);
            if (c == '\n') delta.y -= SIGN(delta.y);
            // printf("&.... carret indx is now %li\n", caret_index);
            continue;
        }

        if (delta.x) {
            if (caret_index == 0 && delta.x < 0) {
                delta.x = 0;
                continue;
            }

            caret_index += SIGN(delta.x);
            delta.x -= SIGN(delta.x);
        }

        break;
    }

    caret_position_px.graft(survey_position(caret_index));
    caret_blink_timer = 0;

    auto editor_height = size->get().y;
    auto caret_base = caret_position_px.y + font_size_px;
    auto the_line_tm = editor_height - scroll_offset->y;

    if (caret_position_px.y < -scroll_offset->y) {
        scroll_offset->y = -caret_position_px.y;
    } else if (caret_base >= the_line_tm) {
        scroll_offset->y = editor_height - caret_base;
    }
}

void TextEdit::save_to_file() {
    file.write(text);
}

void TextEdit::on_wheel(WheelEvent& event) {
    scroll_offset->y += event.delta_y * font_size_px * 5;

    if (scroll_offset->y > 0) scroll_offset->y = 0;
}

void TextEdit::on_tab_focus(TabFocusEvent& event) {
    String title = file.get_path();
    title.append(" - Claire's Editor");
    RayLib::SetWindowTitle(title.as_c());
}

void TextEdit::on_input() {
    bool changes_made = false;

    if (RayLib::IsKeyPressed(RayLib::KEY_S) && RayLib::IsKeyDown(RayLib::KEY_LEFT_CONTROL)) {
        // CTRL+S
        printf("Save! %s\n", file.get_path().as_c());
        save_to_file();
        return;
    }


    char c = '\0';
    while ((c = RayLib::GetCharPressed())) {
        text.insert(c, caret_index);
        changes_made = true;
        move_caret({1, 0});
    }

    if (RayLib::IsKeyTyped(RayLib::KEY_DELETE)) {
        text.remove(caret_index);
        changes_made = true;
    }

    if (RayLib::IsKeyTyped(RayLib::KEY_BACKSPACE)) {
        // Not encompassing due to caret timer
        if (caret_index > 0) {
            text.remove(caret_index - 1);
            changes_made = true;
        }

        move_caret({-1, 0});
    }

    if (RayLib::IsKeyTyped(RayLib::KEY_ENTER)) {
        text.insert('\n', caret_index);
        changes_made = true;
        move_caret({1, 0});
    }

    Vector2 caret_delta = Vector2::zero();
    if (RayLib::IsKeyTyped(RayLib::KEY_LEFT))   caret_delta.x--;
    if (RayLib::IsKeyTyped(RayLib::KEY_RIGHT))  caret_delta.x++;
    if (RayLib::IsKeyTyped(RayLib::KEY_UP))     caret_delta.y--;
    if (RayLib::IsKeyTyped(RayLib::KEY_DOWN))   caret_delta.y++;
    if (caret_delta.x || caret_delta.y) move_caret(caret_delta);

    if (changes_made) {
        caret_blink_timer = 0;
        parser.parse();
    }


    // RayLib::GetCharPressed();
}

void TextEdit::draw_self() {
    caret_blink_timer++;

    bool caret_visible = true;

    if (caret_blink_timer > CARET_BLINK_DURATION) {
        caret_blink_timer = 0;
    } else if (caret_blink_timer > CARET_BLINK_DURATION / 2) {
        caret_visible = false;
    }

    Vector2 pos = get_draw_position();
    RayLib::DrawRectangle(
        pos.x,
        pos.y,
        1,
        size->get().y,
        RayLib::ColorAlpha(Colors::FG.to_ray(), 0.2)
    );

    draw_text();

    if (caret_visible && is_focused()) {
        RayLib::DrawRectangle(
            pos.x + caret_position_px.x + 1,
            pos.y + caret_position_px.y,
            2,
            font_size_px,
            Colors::FG.to_ray()
        );
    }
}

void TextEdit::draw_text_plain_jane() {
    Vector2 pos = get_draw_position();
    std::vector<String> lines = text.split('\n');

    for (size_t i=0; i<lines.size();i++) {
        RayLib::DrawTextEx(
            font,
            lines[i].as_c(),
            { (float)pos.x + 4, (float)pos.y + (font_size_px * i) },
            (float)font_size_px,
            0,
            Colors::FG.to_ray()
        );
    }
}

void TextEdit::draw_text() {
    Vector2 base_pos = get_draw_position();
    Vector2 pointer = base_pos;

    for (auto node : parser.tokens) {
        if (node.type == TokenType::NEWLINE) {
            pointer.y += font_size_px;
            pointer.x = base_pos.x;
            continue;
        }

        RayLib::Color color = Colors::FG.to_ray();

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
                        color = RayLib::BLUE;
                        break;
                    case String::hash("nullptr"):
                        color = RayLib::PURPLE;
                        break;
                }

                break;
            case TokenType::NUMBER:
                color = RayLib::GREEN;
                break;
            case TokenType::COLON:
            case TokenType::SEMICOLON:
            case TokenType::DOT:
                color = RayLib::GRAY;
                break;
            case TokenType::ASTERISK:
            case TokenType::EQUALS:
                color = RayLib::WHITE;
                break;
            case TokenType::OPEN_PAREN:
            case TokenType::CLOSE_PAREN:
            case TokenType::OPEN_BRACE:
            case TokenType::CLOSE_BRACE:
                color = RayLib::SKYBLUE;
            default:
                break;
        }

        if (node.commented) {
            color = RayLib::DARKGREEN;
        }


        RayLib::DrawTextEx(
            font,
            node.text.as_c(),
            { (float)pointer.x + 4, (float)pointer.y},
            (float)font_size_px,
            0,
            color
        );

        pointer.x += RayLib::MeasureTextEx(font, node.text.as_c(), font_size_px, 0).x;
    }
}

Vector2 TextEdit::survey_position(size_t index) {
    Vector2 out = Vector2::zero();

    size_t last_newline = 0;

    printf("===\n");

    for (size_t i = 0; i < index; i++) {
        char c = text.as_c()[i];
        ASSERT(c, "Can't survey the end of time");

        if (c != '\n') continue;

        out.y += font_size_px;
        last_newline = i;
    }

    String partial_line = text.slice(last_newline, index);
    int32_t width = RayLib::MeasureTextEx(font, partial_line.as_c(), font_size_px, 0).x;
    out.x = width;

    return out;
}

void TextEdit::on_click(ClickEvent& event) {
    Vector2 mouse_pos = Vector2::from_ray(RayLib::GetMousePosition());
    mouse_pos = mouse_pos - get_draw_position();

    size_t line_number = mouse_pos.y / font_size_px;
    std::vector<String> lines = text.split('\n');
    if (line_number > lines.size() - 1) line_number = lines.size() - 1;

    String line = lines[line_number];
    int32_t x_left = mouse_pos.x;

    size_t line_length = strlen(line.as_c());
    for (size_t i=1; i < line_length; i++) {
        int32_t width = RayLib::MeasureTextEx(font, line.first_n(i).as_c(), font_size_px, 0).x;

        if (width < x_left && i != line_length - 1) continue;

        caret_position_px.x = width;
        caret_position_px.y = line_number * font_size_px;
        caret_blink_timer = 0;

        caret_index = str_index_from_vec2(text.as_c(), {(int32_t)i, (int32_t)line_number});

        // printf("(%i, %i)\n", caret_position_px.x, caret_position_px.y);
        // printf("%c\n", line.as_c()[i - 1]);
        break;
    }

    printf("%i, %i\n", caret_position_px.x, caret_position_px.y);
}