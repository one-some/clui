#include "UI/TextEdit/TextEdit.h"

void TextEdit::on_input_insert_mode() {
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

    if (RayLib::IsKeyPressed(RayLib::KEY_ESCAPE)) {
        set_edit_mode(EditMode::NORMAL);
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
}

void TextEdit::on_input_command_mode() {
    char c = (char)RayLib::GetCharPressed();
    printf("%c\n", c);
}

void TextEdit::on_input_normal_mode() {
    // FIXME: We should read all in the buffer. We don't.
    char c = (char)RayLib::GetCharPressed();

    switch (c) {
        case 'h':
            move_caret({-1, 0});
            break;
        case 'j':
            move_caret({0, 1});
            break;
        case 'k':
            move_caret({0, -1});
            break;
        case 'l':
            move_caret({1, 0});
            break;
        case 'i':
            set_edit_mode(EditMode::INSERT);
            break;
        case ':':
            set_edit_mode(EditMode::COMMAND);
            break;
        case 'a':
            move_caret({1, 0});
            set_edit_mode(EditMode::INSERT);
            break;
        case 'Z':
            RayLib::CloseWindow();
            break;
        case 'w':
            // Advance word
            advance_caret_word();
            break;
    }
}

void TextEdit::on_input() {
    switch (edit_mode) {
        case EditMode::NORMAL:
            on_input_normal_mode();
            return;
        case EditMode::INSERT:
            on_input_insert_mode();
            return;
        case EditMode::COMMAND:
            on_input_command_mode();
            return;
    }
}
