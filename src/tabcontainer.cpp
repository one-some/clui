#include "tabcontainer.h"
#include "textlabel.h"

void TabContainer::add_tab(const char* label) {
    Button* button = new Button();
    TextLabel* lab = new TextLabel(label);

    lab->position->strategy = PositionStrategy::CENTER;
    lab->color = RayLib::WHITE;
    lab->font_size = 16;

    uint32_t our_idx = buttons.size();

    button->callback_on_click = [=] {
        focus_tab(our_idx);
    };

    button->add_child(lab);
    tab_button_stack.add_child(button);
    buttons.push_back(button);
}