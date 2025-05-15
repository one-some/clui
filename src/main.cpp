#include "assert.h"
#include <stdio.h>
#include "Dep/raylib.h"
#include <cstdint>
#include <vector>
#include <iostream>
#include <memory>
#include <ranges>

#include "UI/Container/Container.h"
#include "textedit.h"
#include "color.h"
#include "cpp.h"
#include "Claire/File.h"
#include "Claire/String.h"
#include "UI/Stack/Stack.h"
#include "tabcontainer.h"
#include "fontglobal.h"
#include "terminal.h"
#include "UI/FileList/FileList.h"

std::unique_ptr<RayLib::Font> Font::the_raw;

class TextureRect : public Container {
    public:
        std::unique_ptr<RayLib::Texture2D> texture;

        TextureRect(const char* file_name) {
            texture = std::make_unique<RayLib::Texture2D>(RayLib::LoadTexture(file_name));
            size->set_raw({ texture->width, texture->height });
        }

        void draw_self() override {
            if (!texture) return;

            Vector2 pos = position->get_global();
            RayLib::DrawTexturePro(
                *texture,
                { 0.0, 0.0, (float)texture->width, (float)texture->height },
                Vector2::to_ray_rect(pos, size->get()),
                { 0, 0 },
                0.0,
                RayLib::WHITE
            );
        }
};

class ColorRect : public Container {
    public:
        RayLib::Color color = RayLib::RED;

        void draw_self() override {
            Vector2 pos = position->get_global();
            RayLib::DrawRectangle(pos.x, pos.y, size->get().x, size->get().y, color);
        }
};

int main() {
    // SetConfigFlags(FLAG_WINDOW_UNDECORATED);
    RayLib::SetTraceLogLevel(RayLib::LOG_ERROR);
    RayLib::SetConfigFlags(RayLib::FLAG_WINDOW_RESIZABLE);
    RayLib::InitWindow(500, 500, "clui test");
    RayLib::SetTargetFPS(60);
    uint64_t frames = 0;

    auto root = Container();
    root.size->set_raw({ 500, 500 });

    auto stack = VStack();
    stack.size->set_raw({300, 300});
    stack.size->strategy_x = SizeStrategy::EXPAND;
    stack.size->strategy_y = SizeStrategy::EXPAND;
    root.add_child(&stack);

    auto sidebar_cont = HStack();
    stack.add_child(&sidebar_cont);

    auto file_list = FileList("./");
    file_list.size->strategy_x = SizeStrategy::FORCE;
    file_list.size->set_x(200);
    sidebar_cont.add_child(&file_list);

    auto tabs = TabContainer();
    sidebar_cont.add_child(&tabs);

    auto term = Terminal();
    tabs.add_child(&term);
    tabs.add_tab("Terminal");

    auto te1 = TextEdit("src/textedit.h");
    tabs.add_child(&te1);
    tabs.add_tab("textedit.h");

    auto te2 = TextEdit("src/textedit.cpp");
    tabs.add_child(&te2);
    tabs.add_tab("src/textedit.cpp");

    auto sc3 = ColorRect();
    sc3.color = Color(0x0000FF).to_ray();
    sc3.size->strategy_y = SizeStrategy::FORCE;
    sc3.size->set_y(20);
    stack.add_child(&sc3);

    while (!RayLib::WindowShouldClose()) {
        root.size->set_x(RayLib::GetRenderWidth());
        root.size->set_y(RayLib::GetRenderHeight());

        root.propagate_mouse_motion({RayLib::GetMouseX(), RayLib::GetMouseY()});
        if (RayLib::IsMouseButtonPressed(0)) root.propagate_click();

        RayLib::BeginDrawing();
        RayLib::ClearBackground(Colors::BG.to_ray());

        // if (frames % 2 == 0) rect->position.x += 1;

        root.draw_tree();

        RayLib::EndDrawing();
        frames++;
    }

    RayLib::CloseWindow();

    return 0;
}
