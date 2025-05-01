#pragma once
#include "container.h"

class Stack : public Container {
    public:
        void draw_self() override {
            // WE ARE EVIL
            reposition_children();
        }

    private:
        virtual void reposition_children() { }

        void on_child_added(Container* child) override {
            reposition_children();
        }
};

class VStack : public Stack {
    private:
        void reposition_children() override;
};

class HStack : public Stack {
    private:
        void reposition_children() override;
};
