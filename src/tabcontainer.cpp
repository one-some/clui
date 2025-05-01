#include "tabcontainer.h"
#include "textlabel.h"

void TabContainer::add_tab(const char* label) {
    Button* button = new Button();
    TextLabel* lab = new TextLabel(label);

    lab->position->strategy = PositionStrategy::CENTER;
    lab->color = Ray::WHITE;
    lab->font_size = 24;
    button->add_child(lab);
    tab_button_stack.add_child(button);
    buttons.push_back(button);
}