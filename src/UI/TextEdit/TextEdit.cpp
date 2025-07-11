#include <cstring>
#include <cstddef>
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

int find_line_start(const String& string, int index) {
    while (index > 0 && string[index - 1] != '\n') {
        index--;
    }
    return index;
}

void TextEdit::move_caret(Vector2 delta) {
    // Let's work on migrating caret stuff to int.
    const int text_length = text.length();
    int working_index = caret_index;
    int line_start = find_line_start(text, caret_index);
    
    printf("\n\nLinestart: %d. Working index (start): %d '%c'\n", line_start, working_index, text[working_index]);

    int line_length = *(text.find("\n", line_start + 1)) - line_start;

    if (text[working_index] == '\n' && delta.x > 0) {
        // JUST THE WORST HACK ON EARTH!!!!!! HELP I CANT CODE!!!!!
    } else if (delta.x) {
        desired_x = working_index - line_start + delta.x;

        if (desired_x < 0) {
            // Too left
            desired_x = 0;
        } else if (desired_x >= line_length) {
            // Too right
            desired_x = line_length;
        }

        working_index = line_start + desired_x;
    }

    if (delta.y > 0) {
        // Down

        while (delta.y > 0) {
            if (text[working_index] == '\n') delta.y--;
            working_index++;
            if (working_index == text_length) break;
        }

        for (int i = 0; i < desired_x; i++) {
            if (text[++working_index] == '\n') break;
        }
    }

    if (delta.y < 0) {
        // Up

        // We've gotta meet two newlines
        delta.y--;
        if (text[working_index] == '\n') delta.y--;

        while (working_index > 0) {
            if (text[working_index] == '\n') delta.y++;
            if (delta.y >= 0) {
                working_index++;
                break;
            }
            working_index--;
        }

        for (int i = 0; i < desired_x; i++) {
            if (text[++working_index] == '\n') break;
        }
    }

    set_caret_index(working_index);
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
        int32_t caret_width = 2;
        int32_t caret_offset = 1;

        if (edit_mode == EditMode::NORMAL) {
            // HACK
            caret_width = RayLib::MeasureTextEx(font, "X", font_size_px, 0).x;
            caret_offset = 3;
        }

        RayLib::DrawRectangle(
            pos.x + caret_position_px.x + caret_offset,
            pos.y + caret_position_px.y,
            caret_width,
            font_size_px,
            Colors::FG.to_ray()
        );
    }

    if (is_hovered()) {
        FrameManager::set_frame_cursor(RayLib::MOUSE_CURSOR_IBEAM);
    }
}

void draw_squiggle(Vector2 start, int32_t target_length, RayLib::Color color) {
    // This is the worst code ive ever written.
    ASSERT(target_length > 1, "be so fr.");

    Vector2 pointer = Vector2(start);

    int32_t squiggle_height = 4;
    int32_t squiggle_length = 4;

    bool in_dip = false;

    while (true) {
        int32_t delta_x = pointer.x - start.x;
        if (delta_x + squiggle_length > target_length)
            squiggle_length = target_length - delta_x;

        RayLib::DrawLine(
            pointer.x,
            pointer.y,
            pointer.x += squiggle_length,
            pointer.y += squiggle_height * (in_dip ? -1 : 1),
            color
        );
        in_dip = !in_dip;

        if (delta_x == target_length) break;
    }
}

void TextEdit::draw_squiggles() {
    Vector2 base_pos = get_draw_position();
    // HACK
    float char_width = RayLib::MeasureTextEx(font, "X", font_size_px, 0).x;

    // NEEDLESSLY EXPENSIVE!!! THIS RUNS EVERY FRAME BUT WHATEVER MAN...
    auto lines = text.split('\n');

    for (auto& diag : get_diagnostics()) {
        for (size_t line = diag.range_start.line; line <= diag.range_end.line; line++) {
            int32_t char_length = lines[line].length();
            Vector2 start = { 0, ((line + 1) * font_size_px) - 4 };

            // Before char_length gets mangled
            if (line == diag.range_end.line) {
                char_length -= (char_length - diag.range_end.col);
            }

            if (line == diag.range_start.line) {
                start.x = diag.range_start.col * char_width;
                char_length -= diag.range_start.col;
            }

            draw_squiggle(base_pos + start, char_length * char_width, RayLib::RED);
        }
    }
}

void TextEdit::draw_selection() {
    if (selection.state == SelectionState::CRITICAL_STATE) return;
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
    draw_squiggles();

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
                    case String::hash("int"):
                    case String::hash("size_t"):
                    case String::hash("switch"):
                    case String::hash("case"):
                    case String::hash("if"):
                    case String::hash("else"):
                    case String::hash("class"):
                    case String::hash("auto"):
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
    if (selection.state == SelectionState::SELECTING)
        selection.state = SelectionState::NORMAL;
}

void TextEdit::on_mouse_down(MouseDownEvent& event) {
    if (event.button != MouseButton::LEFT) return;

    selection.state = SelectionState::CRITICAL_STATE;
    selection.start = move_caret_to_mouse();
}

void TextEdit::on_mouse_move(MouseMoveEvent& event) {
    if (selection.state == SelectionState::CRITICAL_STATE)
        selection.state = SelectionState::SELECTING;
    
    if (selection.state != SelectionState::SELECTING) return;
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

    set_caret_index(i, true);
    return i;
}

void TextEdit::set_caret_index(int index, bool set_desired_x) {
    ASSERT(index < (int)text.length(), "set_caret_index: Too far");

    caret_position_px.graft(survey_position(index));
    caret_blink_timer = 0;
    caret_index = index;

    if (set_desired_x) {
        int line_start = caret_index;

        while (text.as_c()[line_start] != '\n') {
            line_start--;
            if (line_start == 0) break;
        }

        desired_x = caret_index - line_start - 1;
        if (desired_x < 0) desired_x = 0;
    }
}

void TextEdit::on_mouse_hover(MouseHoverEvent& event) {
    if (!event.hovering) {
        hover_info->self_visible = false;
        return;
    }

    Vector2 mouse_pos = Vector2::from_ray(RayLib::GetMousePosition());
    mouse_pos = mouse_pos - get_draw_position();

    if (mouse_pos.y < 0) return;
    if (mouse_pos.x < 0) return;


    hover_label->text = "...";
    hover_info->self_visible = true;
    hover_info->position->set_raw({
        mouse_pos.x + 10,
        mouse_pos.y
    });

    // HACK
    float char_width = RayLib::MeasureTextEx(font, "X", font_size_px, 0).x;

    // It seems there's often a little bit of leeway added to make selection easier
    float biased_mouse_pos = mouse_pos.x - (char_width / 2.0f);
    biased_mouse_pos = max(0.0f, biased_mouse_pos);

    int column = round(biased_mouse_pos / char_width);
    int line = mouse_pos.y / font_size_px;
    LSPClient::the().file_did_hover(
        file.get_path(),
        line,
        column,
        // Maybe pass a callback here?
        [this](const JSONObject& object) {
            printf("%sAAHHHH\n", object.to_string().as_c());
            // If result doesn't exist something has gone wrong... oops!
            auto result_ambigious = object.get("result");

            // Not the best but idk any other way to do it
            if (result_ambigious->is<JSONNull>()) return;

            auto result = result_ambigious->as<JSONObject>();
            auto contents = result->get<JSONObject>("contents");

            String type = contents->get<JSONString>("kind")->value;
            ASSERT(type == "plaintext", "hover: Ok idk what that type '%s' is", type.as_c());

            String value = contents->get<JSONString>("value")->value;
            printf("\nXXX\n%s\n\n", value.as_c());
            hover_label->text = value;
        }
    );
}
