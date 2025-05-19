#include "tabcontainer.h"
#include "UI/TextLabel/TextLabel.h"

void TabContainer::add_tab(const char* label, bool allow_close) {
    uint32_t x_offset = 0;

    for (auto child : tab_button_stack.visible_children()) {
        x_offset += child->size->get().x;
    }

    Button* button = new Button();
    TextLabel* lab = new TextLabel(label);

    lab->position->x_strategy = XPositionStrategy::CENTER;
    lab->position->y_strategy = YPositionStrategy::CENTER;
    lab->color = RayLib::WHITE;
    lab->font_size = 16;

    button->position->set_x(x_offset);
    button->size->set_y(tab_button_stack.size->get().y);

    uint32_t x_padding = 24;
    uint32_t our_idx = tab_button_stack.children.size();

    if (allow_close) {
        Button* close_button = new Button();
        TextLabel* x = new TextLabel("X");

        x->color = RayLib::GRAY;
        x->font_size = 16;
        x->position->x_strategy = XPositionStrategy::CENTER;
        x->position->y_strategy = YPositionStrategy::CENTER;

        close_button->size->set_raw(x->text_bounds() * 1.25);

        close_button->position->x_strategy = XPositionStrategy::RIGHT;
        close_button->position->y_strategy = YPositionStrategy::CENTER;

        close_button->callback_on_click = [our_idx] {
            close_tab(our_idx);
        };

        close_button->add_child(x);
        button->add_child(close_button);
        x_padding += 32;
    }

    button->size->set_x(lab->text_bounds().x + x_padding);

    button->callback_on_click = [=] {
        focus_tab(our_idx);
    };

    button->add_child(lab);
    tab_button_stack.add_child(button);
}