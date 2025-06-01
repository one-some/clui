#include "UI/TabContainer/TabContainer.h"
#include "UI/TextLabel/TextLabel.h"

void TabContainer::recalculate_tab_positions() {
    uint32_t x_offset = 0;
    for (auto child : tab_button_stack->visible_children()) {
        child->position->set_x(x_offset);
        x_offset += child->size->get().x;
    }
}

Tab* TabContainer::add_tab(const char* label, std::unique_ptr<Container> view, bool allow_close) {
    tabs.push_back(std::make_unique<Tab>(
        label,
        view.get()
    ));
    add_child(std::move(view));

    Tab& tab = *(tabs.back());
    if (!active_tab) active_tab = tabs.back().get();

    auto button = tab_button_stack->create_child<Button>();
    button->size->set_y(tab_button_stack->size->get().y);

    recalculate_tab_positions();

    auto lab = button->create_child<TextLabel>(label);
    lab->position->x_strategy = XPositionStrategy::CENTER;
    lab->position->y_strategy = YPositionStrategy::CENTER;
    lab->color = RayLib::WHITE;
    lab->font_size = 16;

    uint32_t x_padding = 24;

    if (allow_close) {
        auto close_button = button->create_child<Button>();
        auto x = close_button->create_child<TextLabel>("X");

        x->color = RayLib::GRAY;
        x->font_size = 16;
        x->position->x_strategy = XPositionStrategy::CENTER;
        x->position->y_strategy = YPositionStrategy::CENTER;

        close_button->size->set_raw(x->text_bounds() * 1.25);

        close_button->position->x_strategy = XPositionStrategy::RIGHT;
        close_button->position->y_strategy = YPositionStrategy::CENTER;

        close_button->callback_on_click = [this, &tab, button] {
            auto tab_iterator = std::find_if(
                tabs.begin(),
                tabs.end(),
                [&tab](const std::unique_ptr<Tab>& t) {
                    return t.get() == &tab;
                }
            );
            
            ASSERT(tab_iterator != tabs.end(), "Expected the tab to be liek....real....");

            if (this->active_tab == &tab) {
                // We are the active tab... lets do something.... ANYTHING!!
                if (tabs.size() > 1) {
                    auto next_up = std::next(tab_iterator);

                    // If next won't work try previous
                    if (next_up == tabs.end()) next_up = std::prev(tab_iterator);
                    ASSERT(next_up != tabs.end(), "Okay what's going on here");

                    active_tab = next_up->get();
                } else {
                    // Uh oh
                    active_tab = nullptr;
                }
            }

            tab_button_stack->remove_child(button);

            recalculate_tab_positions();

            tabs.erase(std::remove_if(
                tabs.begin(),
                tabs.end(),
                [&tab](const std::unique_ptr<Tab>& t) {
                    return t.get() == &tab;
                }),
                tabs.end()
            );
        };

        x_padding += 32;
    }

    button->size->set_x(lab->text_bounds().x + x_padding);

    button->callback_on_click = [this, &tab] {
        this->active_tab = &tab;
    };

    return &tab;
}