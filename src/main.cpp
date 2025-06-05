#include "assert.h"
#include <stdio.h>
#include "Dep/raylib.h"
#include <cstdint>
#include <vector>
#include <iostream>
#include <memory>
#include <ranges>
#include <unistd.h>
#include "FrameManager/FrameManager.h"
#include "Claire/JSON/JSON.h"

#include "UI/Container/Container.h"
#include "color.h"
#include "cpp.h"
#include "LSPClient/LSPClient.h"
#include "Claire/File.h"
#include "Claire/String.h"
#include "Actions/EditorActions.h"
#include "UI/Stack/Stack.h"
#include "UI/TabContainer/TabContainer.h"
#include "fontglobal.h"
#include "terminal.h"
#include "UI/FileList/FileList.h"
#include <fcntl.h>
#include "log.h"

std::unique_ptr<RayLib::Font, RLFontDeleter> Font::the_raw;

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

void reload_self(int argc, char *argv[], char *envp[]) {
    printf("ROFL RELOAD\n");

    int fd = open("/proc/self/exe", O_RDONLY | O_CLOEXEC);
    ASSERT(fd != -1, "Coudln't open self");

    std::vector<char*> args;
    for (int i = 0; i < argc;  i++) {
        args.push_back(argv[i]);
    }
    args.push_back(NULL);

    fexecve(fd, args.data(), envp);

    ASSERT(false, "FAILED TO SHIFT EXEC");
}

int main(int argc, char *argv[], char *envp[]) {
    auto lsp_client = new LSPClient();
    lsp_client->init();

    LogContainer::swallow_stdout();

    printf("Hello\n");

    RayLib::SetTraceLogLevel(RayLib::LOG_ERROR);
    RayLib::SetConfigFlags(RayLib::FLAG_WINDOW_RESIZABLE);
    RayLib::InitWindow(500, 500, "clui test");

    RayLib::SetWindowIcon(RayLib::LoadImage("claire_head.png"));

    RayLib::SetTargetFPS(60);
    uint64_t frames = 0;
    
    auto root = Container();
    FrameManager::root_container = &root;
    root.size->set_raw({ 500, 500 });

    auto sidebar_cont = root.create_child<HStack>();
    sidebar_cont->allow_user_resize = true;
    sidebar_cont->size->strategy_x = SizeStrategy::EXPAND_TO_FILL;
    sidebar_cont->size->strategy_y = SizeStrategy::EXPAND_TO_FILL;

        auto file_list = sidebar_cont->create_child<FileList>("./");
        file_list->size->strategy_x = SizeStrategy::FORCE;
        file_list->size->set_x(200);

        auto tabs_terminal_stack = sidebar_cont->create_child<VStack>();
        tabs_terminal_stack->debug_name = String("tabs_terminal_stack");
        tabs_terminal_stack->allow_user_resize = true;
        tabs_terminal_stack->size->strategy_x = SizeStrategy::EXPAND_TO_FILL;
        tabs_terminal_stack->size->strategy_y = SizeStrategy::EXPAND_TO_FILL;

            auto tabs = tabs_terminal_stack->create_child<TabContainer>();
            tabs->size->strategy_x = SizeStrategy::EXPAND_TO_FILL;
            tabs->size->strategy_y = SizeStrategy::EXPAND_TO_FILL;
            EditorActions::register_primary_tab_container(tabs);
            EditorActions::open_file_in_new_tab("src/main.cpp");
            // EditorActions::open_file_in_new_tab("src/textedit.cpp");
            
            auto bottom_tabs = tabs_terminal_stack->create_child<TabContainer>();
            bottom_tabs->debug_name = String("bottom_tabs");
            bottom_tabs->size->strategy_y = SizeStrategy::FORCE;
            bottom_tabs->size->set_y(200);

                auto term = std::make_unique<Terminal>();
                bottom_tabs->add_tab("Terminal", std::move(term));

                auto log = std::make_unique<LogContainer>();
                bottom_tabs->add_tab("Babble", std::move(log));

                auto issues = std::make_unique<Container>();
                bottom_tabs->add_tab("Code Issues", std::move(issues));


    while (!RayLib::WindowShouldClose()) {
        FrameManager::set_frame_cursor(RayLib::MouseCursor::MOUSE_CURSOR_DEFAULT);

        root.size->set_x(RayLib::GetRenderWidth());
        root.size->set_y(RayLib::GetRenderHeight());

        if (RayLib::IsKeyPressed(RayLib::KEY_RIGHT_SHIFT)) {
            reload_self(argc, argv, envp);
        }

        root.propagate_mouse_motion({RayLib::GetMouseX(), RayLib::GetMouseY()});
        if (RayLib::IsMouseButtonPressed(0)) root.propagate_click();

        if (Container::focused_element) {
            Container::focused_element->on_input();

            float wheel_delta = RayLib::GetMouseWheelMove();
            if (wheel_delta) {
                Container::focused_element->on_wheel(wheel_delta);
            }
        }

        RayLib::BeginDrawing();
        RayLib::ClearBackground(Colors::BG.to_ray());

        // if (frames % 2 == 0) rect->position.x += 1;

        root.draw_tree();
        FrameManager::draw_debug_points();

        RayLib::EndDrawing();

        RayLib::AwesomeSetMouseCursor(FrameManager::get_frame_cursor());

        FrameManager::run_queued_operations();
        LogContainer::flush_stdout();

        frames++;
    }

    RayLib::CloseWindow();

    return 0;
}
