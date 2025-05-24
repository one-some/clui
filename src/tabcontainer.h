#pragma once
#include <vector>
#include <memory>
#include <stdarg.h>
#include <cstdio>
#include "UI/Container/Container.h"
#include "UI/Stack/Stack.h"
#include "UI/Button/Button.h"
#include "Claire/Assert.h"

class Tab {
    public:
        const char* label;
        Container* view;

        Tab(const char* _label, Container* _view): label(_label), view(_view) {
        }
};

class TabContainer : public VStack {
    public:
        TabContainer() {
            tab_button_stack.size->set_y(30);
            tab_button_stack.size->strategy_y = SizeStrategy::FORCE;
            add_child(std::make_unique<Container>(tab_button_stack));
        }

        void add_tab(const char* label, Container* view, bool allow_close = false);

        void draw_tree() override {
            draw_self();

            tab_button_stack.draw_tree();

            if (!active_tab) return;
            active_tab->view->draw_tree();
        }

        std::vector<Container*> visible_children() override {
            std::vector<Container*> out;

            if (children.empty()) return out;
            out.push_back(children[0].get());

            if (!active_tab) return out;

            out.push_back(active_tab->view);
            return out;
        }

    private:
        Container tab_button_stack;
        std::vector<std::unique_ptr<Tab>> tabs;
        Tab* active_tab = nullptr;
};