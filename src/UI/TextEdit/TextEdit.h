// Hello from CLAIRE Hello From 
//STACK

#pragma once
#include "UI/Container/Container.h"
#include "Claire/String.h"
#include "Claire/File.h"
#include "Claire/Math.h"
#include "cpp.h"
#include "fontglobal.h"

class TextCoordinate {
public:
    size_t line = 0;
    size_t col = 0;
};

class EditorDiagnostic {
public:
    String message;
    String file_path;
    TextCoordinate range_start;
    TextCoordinate range_end;

    EditorDiagnostic(
        String message,
        String file_path,
        TextCoordinate range_start,
        TextCoordinate range_end
    ) : message(message), file_path(file_path), range_start(range_start), range_end(range_end) { }

    String label() const {
        return file_path + ": " + message;
    }
};

enum class SelectionState {
    NORMAL,
    // CRITICAL_STATE: We've clicked, but the mouse has not yet moved, so end hasn't updated.
    CRITICAL_STATE,
    // Time to move!
    SELECTING,
};

class TextSelection {
public:
    size_t start = 0;
    size_t end = 0;

    SelectionState state = SelectionState::NORMAL;

    TextSelection rectified() {
        auto copy = *this;
        if (copy.start > copy.end) std::swap(copy.start, copy.end);
        return copy;
    }

    inline bool empty() {
        return start == end;
    }

    inline bool reversed() {
        return start > end;
    }

    inline size_t length() {
        return abs((int32_t)(end - start));
    }

    // Maybe inconsistant api design......
    void clear() {
        start = 0;
        end = 0;
    }
};

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
        TextSelection selection;

        int32_t font_size_px = 16;

        static const int32_t CARET_BLINK_DURATION = 530 / 8;

        TextEdit(String path) : file(path) {
            register_class_handler<MouseDownEvent, TextEdit>(&TextEdit::on_mouse_down);
            register_class_handler<MouseUpEvent, TextEdit>(&TextEdit::on_mouse_up);
            register_class_handler<MouseMoveEvent, TextEdit>(&TextEdit::on_mouse_move);
            register_class_handler<TabFocusEvent, TextEdit>(&TextEdit::on_tab_focus);

            scroll_behavior = ScrollBehavior::VERTICAL;

            // printf("Waiiit....Hello from crazy world... '%s'\n", file.get_path().as_c());
            text = file.read();
            parser = CPPParser(&text);
            parser.parse();
        }

        void draw_self() override;
        void move_caret(Vector2 delta);
        void save_to_file();
        static size_t str_index_from_vec2(const char* text, Vector2 vec);
    
    private:
        void on_mouse_down(MouseDownEvent& event);
        void on_mouse_up(MouseUpEvent& event);
        void on_mouse_move(MouseMoveEvent& event);
        void on_wheel(WheelEvent& event);
        void on_tab_focus(TabFocusEvent& event);

        void on_input() override;
        void draw_text();

        size_t move_caret_to_mouse();
        void draw_selection();
        void delete_selected_text();
        void set_caret_index(size_t index);
        String get_selected_text();

        void handle_backspace();

        Vector2 survey_position(size_t index);

        void draw_squiggles();

        std::vector<EditorDiagnostic> get_diagnostics() {
            static std::vector<EditorDiagnostic> hack = {EditorDiagnostic("main.cpp", "There's a probvlem ok", {1, 0}, {10, 5})};
            return hack;
        }
};