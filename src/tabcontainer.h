#pragma once
#include <vector>
#include <memory>
#include <stdarg.h>
#include <cstdio>
#include "UI/Container/Container.h"
#include "UI/Stack/Stack.h"
#include "UI/Button/Button.h"
#include "assert.h"

class TabContainer : public VStack {
    public:
        HStack tab_button_stack;
        std::vector<Button*> buttons;

        TabContainer() {
            tab_button_stack.size->set_y(30);
            tab_button_stack.size->strategy_y = SizeStrategy::FORCE;
            add_child(&tab_button_stack);
        }

        void add_tab(const char* label);

        void draw_tree() override {
            draw_self();

            children[0]->draw_tree();
            ASSERT(active_tab_idx + 1 < children.size(), "Please be smaller");
            children[active_tab_idx + 1]->draw_tree();
        }

        void focus_tab(uint32_t size) {
            active_tab_idx = size;
        }

        std::vector<Container*> visible_children() override {
            if (children.size() < 2) return {children[0]};

            return {children[0], children[active_tab_idx + 1]};
        }

    private:
        uint32_t active_tab_idx = 0;
};