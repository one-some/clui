#pragma once
#include <vector>
#include <memory>
#include <stdarg.h>
#include <cstdio>
#include "container.h"
#include "stack.h"
#include "button.h"
#include "assert.h"

class TabContainer : public VStack {
    public:
        HStack tab_button_stack;
        std::vector<Button*> buttons;
        uint32_t active_tab_idx = 0;

        TabContainer() {
            tab_button_stack.size->set_y(30);
            tab_button_stack.size->strategy_y = SizeStrategy::FORCE;
            add_child(&tab_button_stack);

            add_tab("textedit.h");
        }

        void add_tab(const char* label);

        void draw_tree() override {
            children[0]->draw_tree();
            ASSERT(active_tab_idx + 1 < children.size(), "Please be smaller");
            children[active_tab_idx + 1]->draw_tree();
        }
};