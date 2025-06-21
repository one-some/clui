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

void TextEdit::on_tab_focus(TabFocusEvent& event) {
    String title = file.get_path();
    title.append(" - Claire's Editor");
    RayLib::SetWindowTitle(title.as_c());
}

void TextEdit::delete_selected_text() {
    auto rectified_selection = selection.rectified();
    text.slice_inplace(rectified_selection.start, rectified_selection.end);

    if (!selection.reversed()) set_caret_index(caret_index - rectified_selection.length());

    selection.clear();
}

void TextEdit::handle_backspace() {
    // Remove selection
    if (!selection.empty()) {
        delete_selected_text();
        return;
    }

    // Don't backspace the beginning of time
    if (caret_index <= 0) return;

    // Tab backspace
    if (text.slice(caret_index - 4, caret_index) == "    ") {
        for (int i = 0; i < 4; i++) {
            // TODO: Use slice inplace
            text.remove(caret_index - 1);
            set_caret_index(caret_index - 1);
        }
        return;
    }


    text.remove(caret_index - 1);
    set_caret_index(caret_index - 1);
}

String TextEdit::get_selected_text() {
    auto rectified = selection.rectified();
    return text.slice(rectified.start, rectified.end);
}

void TextEdit::on_input() {
    bool changes_made = false;

    if (RayLib::IsKeyDown(RayLib::KEY_LEFT_CONTROL)) {
        if (RayLib::IsKeyPressed(RayLib::KEY_S)) {
            printf("Save! %s\n", file.get_path().as_c());
            save_to_file();
        }

        if (RayLib::IsKeyPressed(RayLib::KEY_C)) {
            auto selected_text = get_selected_text();
            RayLib::SetClipboardText(selected_text.as_c());
        }

        if (RayLib::IsKeyPressed(RayLib::KEY_V)) {
            String clipboard = RayLib::GetClipboardText();

            text.insert(clipboard, caret_index);
            set_caret_index(caret_index + clipboard.length());
            changes_made = true;
        }
    }

    char c = '\0';
    while ((c = (char)RayLib::GetCharPressed())) {
        if (!selection.empty()) delete_selected_text();

        text.insert(c, caret_index);
        changes_made = true;
        set_caret_index(caret_index + 1);
    }

    if (RayLib::IsKeyTyped(RayLib::KEY_DELETE)) {
        text.remove(caret_index);
        changes_made = true;
    }

    if (RayLib::IsKeyTyped(RayLib::KEY_BACKSPACE)) {
        handle_backspace();
        changes_made = true;
    }

    if (RayLib::IsKeyTyped(RayLib::KEY_ENTER)) {
        text.insert('\n', caret_index);
        changes_made = true;
        move_caret({1, 0});
    }

    if (RayLib::IsKeyTyped(RayLib::KEY_TAB)) {
        text.insert("    ", caret_index);
        set_caret_index(caret_index + 4);
        changes_made = true;
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

    draw_text();

    Vector2 pos = get_draw_position();
    if (caret_visible && is_focused()) {
        RayLib::DrawRectangle(
            pos.x + caret_position_px.x + 1,
            pos.y + caret_position_px.y,
            2,
            font_size_px,
            Colors::FG.to_ray()
        );
    }

    if (is_hovered()) {
        FrameManager::set_frame_cursor(RayLib::MOUSE_CURSOR_IBEAM);
    }
}

void TextEdit::draw_selection() {
    if (selection.empty()) return;
    auto rectified_selection = selection.rectified();

    Vector2 base_pos = get_draw_position();
    float char_width = RayLib::MeasureTextEx(font, "X", font_size_px, 0).x;

    Optional<Vector2> rectangle_start;
    size_t row = 0;
    size_t col = 0;
    size_t unrectified_end_row_px = 0;

    for (size_t i = 0; i <= rectified_selection.end; i++) {
        // Track the row where the user's cursor is on for scrolling
        if (i == selection.end) unrectified_end_row_px = row * font_size_px;

        // Make a new rectangle if we just started OR we need to continue one
        // from the last line.
        bool in_middle = (i > rectified_selection.start && i < rectified_selection.end);
        if (i == rectified_selection.start || (col == 0 && in_middle)) {
            rectangle_start = Optional<Vector2>({
                (int32_t)(col * char_width),
                (int32_t)(row * font_size_px)
            });
        }

        bool is_newline = text.as_c()[i] == '\n';
        if (rectangle_start && (is_newline || i == rectified_selection.end)) {
            int32_t width = (char_width * col) - rectangle_start->x;
            RayLib::DrawRectangle(
                base_pos.x + rectangle_start->x,
                base_pos.y + rectangle_start->y,
                width,
                font_size_px,
                RayLib::RED
            );

            rectangle_start = Optional<Vector2>();
        }


        if (is_newline) {
            row++;
            col = 0;
        } else {
            col++;
        }
    }

    // Let's scroll a bit if we need to...
    float bottom_overshoot = (float)unrectified_end_row_px - (float)(size->get().y - base_pos.y);
    float top_overshoot = (float)-scroll_offset->y - (float)unrectified_end_row_px;
    if (bottom_overshoot > 0) {
        scroll_offset->y -= bottom_overshoot / 4.0;
    } else if (top_overshoot > 0) {
        scroll_offset->y += top_overshoot / 4.0;
    }
}

void TextEdit::draw_text() {
    Vector2 base_pos = get_draw_position();

    draw_selection();

    // Nodes
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

        pointer.x += (int32_t)RayLib::MeasureTextEx(font, node.text.as_c(), (float)font_size_px, 0.0f).x;
    }
}

Vector2 TextEdit::survey_position(size_t index) {
    Vector2 out = Vector2::zero();

    size_t last_newline = 0;

    for (size_t i = 0; i < index; i++) {
        char c = text.as_c()[i];
        ASSERT(c, "Can't survey the end of time");

        if (c != '\n') continue;

        out.y += font_size_px;
        last_newline = i;
    }

    String partial_line = text.slice(last_newline, index);
    int32_t width = (int32_t)RayLib::MeasureTextEx(font, partial_line.as_c(), font_size_px, 0).x;
    out.x = width;

    return out;
}

void TextEdit::on_mouse_up(MouseUpEvent& event) {
    selection.complete = true;
}

void TextEdit::on_mouse_down(MouseDownEvent& event) {
    if (event.button != MouseButton::LEFT) return;
    selection.complete = false;
    selection.start = move_caret_to_mouse();
}

void TextEdit::on_mouse_move(MouseMoveEvent& event) {
    if (selection.complete) return;
    selection.end = move_caret_to_mouse();
}

size_t TextEdit::move_caret_to_mouse() {
    Vector2 mouse_pos = Vector2::from_ray(RayLib::GetMousePosition());
    mouse_pos = mouse_pos - get_draw_position();

    if (mouse_pos.y < 0) {
        set_caret_index(0);
        return 0;
    }

    // HACK
    float char_width = RayLib::MeasureTextEx(font, "X", font_size_px, 0).x;

    // It seems there's often a little bit of leeway added to make selection easier
    float biased_mouse_pos = mouse_pos.x - (char_width / 2.0f);
    biased_mouse_pos = max(0.0f, biased_mouse_pos);

    size_t cols_left = round(biased_mouse_pos / char_width);
    size_t rows_left = mouse_pos.y / font_size_px;
    size_t i = 0;
    while (i < text.length()) {
        if (rows_left) {
            if (text.as_c()[i] == '\n') rows_left--;
        } else if (cols_left) {
            // If we get a newline from our x position it means our x is greater
            // than our line width. Just stop at the end of the line.
            if (text.as_c()[i] == '\n') break;
            cols_left--;
        } else {
            break;
        }

        i++;
    }

    set_caret_index(i);
    return i;
}

void TextEdit::set_caret_index(size_t index) {
    caret_position_px.graft(survey_position(index));
    caret_blink_timer = 0;
    caret_index = index;
}