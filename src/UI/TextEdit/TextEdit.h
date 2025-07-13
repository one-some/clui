// Hello from CLAIRE Hello From 
//STACK

#pragma once
#include "UI/Container/Container.h"
#include "Claire/String.h"
#include "Claire/File.h"
#include "Claire/Math.h"
#include "cpp.h"
#include "fontglobal.h"
#include "LSPClient/LSPClient.h"
#include "UI/TextEdit/EditorDiagnostic.h"
#include "UI/TextLabel/TextLabel.h"

enum class EditMode {
    NORMAL,
    INSERT,
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
        EditMode edit_mode = EditMode::NORMAL;

        //String("Hello Folks\nFus Ro Dah.\nClaire speaking.\nLet's code something fantastic....!!!\nLove u jamie");
        static RayLib::Font font;

        Vector2 caret_position_px = {0, 0};
        size_t target_caret_x = 0;
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
            register_class_handler<MouseHoverEvent, TextEdit>(&TextEdit::on_mouse_hover);
            scroll_behavior = ScrollBehavior::VERTICAL;

            hover_info = create_child<Container>();
            hover_info->decoration = std::make_unique<ContainerDecoration>();
            hover_info->decoration->enable_bg = true;
            hover_info->decoration->bg_color = Color(0x181818);
            hover_info->size->set_raw({100, 100});
            hover_info->self_visible = false;

            hover_label = hover_info->create_child<TextLabel>("Hello");
            hover_label->color = Colors::FG.to_ray();
            hover_label->font_size = font_size_px;

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
        int desired_x = 0;

        Container* hover_info = nullptr;
        TextLabel* hover_label = nullptr;

        void on_mouse_down(MouseDownEvent& event);
        void on_mouse_up(MouseUpEvent& event);
        void on_mouse_move(MouseMoveEvent& event);
        void on_wheel(WheelEvent& event);
        void on_tab_focus(TabFocusEvent& event);
        void on_mouse_hover(MouseHoverEvent& event);

        void on_input() override;
        void draw_text();

        void on_input_insert_mode();
        void on_input_normal_mode();

        void set_edit_mode(EditMode mode) {
            caret_blink_timer = 0;
            edit_mode = mode;
        }

        size_t move_caret_to_mouse();
        void draw_selection();
        void delete_selected_text();
        void set_caret_index(int index, bool set_desired_x = false);
        void advance_caret_word();
        bool caret_at_end();
        String get_selected_text();

        void handle_backspace();

        Vector2 survey_position(size_t index);

        void draw_squiggles();

        std::vector<EditorDiagnostic> get_diagnostics() {
            return LSPClient::the().diagnostics_for(file.get_path());
        }
};