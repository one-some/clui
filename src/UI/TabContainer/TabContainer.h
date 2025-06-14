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

        Tab(const char* _label, Container* _view): label(_label), view(_view) { }
};

class TabContainer : public VStack {
    public:
        std::vector<std::unique_ptr<Tab>> tabs;

        TabContainer() {
            auto unique_tabs = std::make_unique<Container>();
            unique_tabs->size->set_y(30);
            unique_tabs->size->strategy_y = SizeStrategy::FORCE;

            tab_button_stack = unique_tabs.get();
            add_child(std::move(unique_tabs));
        }

        Tab* add_tab(const char* label, std::unique_ptr<Container> view, bool allow_close = false);
        void recalculate_tab_positions();

        void draw_tree(Optional<RayLib::Rectangle> parent_scissor) override {
            draw_self();

            tab_button_stack->draw_tree(parent_scissor);

            if (!active_tab) return;
            active_tab->view->draw_tree(parent_scissor);
        }

        std::vector<Container*> visible_children() override {
            std::vector<Container*> out;

            if (children.empty()) return out;
            out.push_back(children[0].get());

            if (!active_tab) return out;

            out.push_back(active_tab->view);
            return out;
        }

        void focus_tab(Tab* tab) {
            active_tab = tab;

            auto event = TabFocusEvent();
            tab->view->dispatch_event(event);
        }

    private:
        Container* tab_button_stack;
        Tab* active_tab = nullptr;
};