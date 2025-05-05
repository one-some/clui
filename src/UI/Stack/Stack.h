#pragma once
#include <cstdio>
#include "UI/Container/Container.h"

class Stack : public Container {
    public:
        void draw_self() override {
            // WE ARE EVIL
            reposition_children();
        }

    protected:
        virtual void reposition_children() = 0;

        void on_child_added(Container* child) override {
            reposition_children();
        }
};

class VStack : public Stack {
    protected:
        void reposition_children() override;
};

class HStack : public Stack {
    protected:
        void reposition_children() override;
};
