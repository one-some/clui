#include "assert.h"
#include <stdio.h>
#include "Dep/raylib.h"
#include <cstdint>
#include <vector>
#include <iostream>
#include <memory>
#include <ranges>
#include <unistd.h>

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

void reload_self() {
    printf("ROFL RELOAD\n");

    char buf[1024];
    ssize_t length = readlink("/proc/self/exe", buf, sizeof(buf) - 1);
    ASSERT(length != -1, "Unable to read self exec link");
    buf[length] = '\0';

    printf("%s\n", buf);
    execl(buf, buf, NULL);
}

int main() {
    RayLib::SetTraceLogLevel(RayLib::LOG_ERROR);
    RayLib::SetConfigFlags(RayLib::FLAG_WINDOW_RESIZABLE);
    RayLib::InitWindow(500, 500, "clui test");
    RayLib::SetTargetFPS(60);
    uint64_t frames = 0;

    auto root = Container();
    root.size->set_raw({ 500, 500 });

    auto sidebar_cont = HStack();
    sidebar_cont.size->strategy_x = SizeStrategy::EXPAND;
    sidebar_cont.size->strategy_y = SizeStrategy::EXPAND;
    root.add_child(&sidebar_cont);

        auto file_list = FileList("./");
        file_list.size->strategy_x = SizeStrategy::FORCE;
        file_list.size->set_x(200);
        sidebar_cont.add_child(&file_list);

        auto tabs_terminal_stack = VStack();
        tabs_terminal_stack.size->strategy_x = SizeStrategy::EXPAND;
        tabs_terminal_stack.size->strategy_y = SizeStrategy::EXPAND;
        sidebar_cont.add_child(&tabs_terminal_stack);


            auto tabs = TabContainer();
            tabs.size->strategy_x = SizeStrategy::EXPAND;
            tabs_terminal_stack.add_child(&tabs);

                auto te1 = TextEdit("src/textedit.h");
                tabs.add_child(&te1);
                tabs.add_tab("textedit.h");

                auto te2 = TextEdit("src/textedit.cpp");
                tabs.add_child(&te2);
                tabs.add_tab("src/textedit.cpp");
            
            auto term = Terminal();
            term.size->strategy_y = SizeStrategy::FORCE;
            term.size->set_y(200);
            tabs_terminal_stack.add_child(&term);


    while (!RayLib::WindowShouldClose()) {
        root.size->set_x(RayLib::GetRenderWidth());
        root.size->set_y(RayLib::GetRenderHeight());

        if (RayLib::IsKeyPressed(RayLib::KEY_RIGHT_SHIFT)) {
            reload_self();
        }

        root.propagate_mouse_motion({RayLib::GetMouseX(), RayLib::GetMouseY()});
        if (RayLib::IsMouseButtonPressed(0)) root.propagate_click();

        if (Container::focused_element) {
            Container::focused_element->on_input();
        }

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
