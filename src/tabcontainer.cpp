#include "tabcontainer.h"
#include "UI/TextLabel/TextLabel.h"

void TabContainer::add_tab(const char* label, Container* view, bool allow_close) {
    add_child(view);

    uint32_t x_offset = 0;

    for (auto child : tab_button_stack.visible_children()) {
        x_offset += child->size->get().x;
    }


    tabs.push_back(std::make_unique<Tab>(
        label,
        view
    ));

    Tab& tab = *(tabs.back());
    if (!active_tab) active_tab = tabs.back().get();

    TextLabel* lab = new TextLabel(label);

    lab->position->x_strategy = XPositionStrategy::CENTER;
    lab->position->y_strategy = YPositionStrategy::CENTER;
    lab->color = RayLib::WHITE;
    lab->font_size = 16;

    tab.button.position->set_x(x_offset);
    tab.button.size->set_y(tab_button_stack.size->get().y);

    uint32_t x_padding = 24;

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

        close_button->callback_on_click = [this, &tab] {
            if (this->active_tab == &tab) {
                // TODO: Select next
                this->active_tab = nullptr;
            }

            tabs.erase(std::remove_if(
                tabs.begin(),
                tabs.end(),
                [&tab](const std::unique_ptr<Tab>& t) {
                    return t.get() == &tab;
                }),
            tabs.end());
            // tab dangles after here.... duh
        };

        close_button->add_child(x);
        tab.button.add_child(close_button);
        x_padding += 32;
    }

    tab.button.size->set_x(lab->text_bounds().x + x_padding);

    tab.button.callback_on_click = [this, &tab] {
        this->active_tab = &tab;
    };

    tab.button.add_child(lab);
    tab_button_stack.add_child(&tab.button);
}