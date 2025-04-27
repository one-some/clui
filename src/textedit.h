#pragma once
#include "container.h"
#include "string.h"

class TextEdit : public Container {
    public:
        String text = String("Hello Folks\nFus Ro Dah.\nClaire speaking.\nLet's code something fantastic....!!!\nLove u jamie");
        Ray::Font* font = nullptr;

        virtual void draw_self();
    
    private:
        void on_click() override;
};