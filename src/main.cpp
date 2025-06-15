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
#include "Claire/Utilization.h"
#include "Project/Project.h"
#include "UI/Container/Container.h"
#include "color.h"
#include "cpp.h"
#include "LSPClient/LSPClient.h"
#include "Claire/File.h"
#include "Claire/String.h"
#include "Actions/EditorActions.h"
#include "UI/Stack/Stack.h"
#include "UI/TabContainer/TabContainer.h"
#include "UI/IssueContainer/IssueContainer.h"
#include "fontglobal.h"
#include "terminal.h"
#include "UI/FileList/FileList.h"
#include <fcntl.h>
#include "log.h"

std::unique_ptr<RayLib::Font, RLFontDeleter> Font::the_raw;

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
    LogContainer::swallow_stdout();

    printf("Hello from claire\n");
    
    LSPClient::the().init();

    RayLib::SetTraceLogLevel(RayLib::LOG_ERROR);
    RayLib::SetConfigFlags(RayLib::FLAG_WINDOW_RESIZABLE);
    RayLib::InitWindow(500, 500, "clui test");

    auto icon = RayLib::SmartImage(Path::exec_relative("claire_head.png").as_c());
    RayLib::SetWindowIcon(icon.image);

    RayLib::SetTargetFPS(60);
    uint64_t frames = 0;
    
    auto project = Project(Path::dir_path(Path::exec_path()));

    auto root = Container();
    root.debug_name = "root";
    FrameManager::root_container = &root;
    root.size->set_raw({ 500, 500 });

    // Apparently ? needs escaping... huh.
    auto ram_usage_label = root.create_child<TextLabel>("(\?\?\?)");
    ram_usage_label->color = Colors::FG.to_ray();
    ram_usage_label->position->y_strategy = YPositionStrategy::BOTTOM;
    ram_usage_label->font_size = 16;

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
            EditorActions::open_file_in_new_tab(Path::exec_relative("src/main.cpp"));
            // EditorActions::open_file_in_new_tab("src/textedit.cpp");
            if (argc > 1) EditorActions::open_file_in_new_tab(argv[1]);
            
            auto bottom_tabs = tabs_terminal_stack->create_child<TabContainer>();
            bottom_tabs->debug_name = String("bottom_tabs");
            bottom_tabs->size->strategy_x = SizeStrategy::EXPAND_TO_FILL;
            bottom_tabs->size->strategy_y = SizeStrategy::FORCE;
            bottom_tabs->size->set_y(200);

                auto term = std::make_unique<Terminal>();
                bottom_tabs->add_tab("Terminal", std::move(term));

                auto log = std::make_unique<LogContainer>();
                bottom_tabs->add_tab("Babble", std::move(log));

                auto issues = std::make_unique<IssueContainer>();
                bottom_tabs->add_tab("Code Issues", std::move(issues));


    while (!RayLib::WindowShouldClose()) {
        FrameManager::set_frame_cursor(RayLib::MouseCursor::MOUSE_CURSOR_DEFAULT);

        root.size->set_x(RayLib::GetRenderWidth());
        root.size->set_y(RayLib::GetRenderHeight());

        if (RayLib::IsKeyPressed(RayLib::KEY_RIGHT_SHIFT)) {
            reload_self(argc, argv, envp);
        }

        auto motion_event = MouseMotionEvent(RayLib::GetMouseX(), RayLib::GetMouseY());
        root.dispatch_event(motion_event);

        if (RayLib::IsMouseButtonPressed(0)) {
            auto click_event = ClickEvent();
            root.dispatch_event(click_event);
        }

        Vector2 wheel_vec = Vector2::from_ray(RayLib::GetMouseWheelMoveV());
        if (RayLib::IsKeyDown(RayLib::KEY_LEFT_SHIFT) || RayLib::IsKeyDown(RayLib::KEY_RIGHT_SHIFT)) {
            wheel_vec.x += wheel_vec.y;
            wheel_vec.y = 0;
        }

        if (wheel_vec) {
            auto wheel_event = WheelEvent({wheel_vec.x, wheel_vec.y});
            root.dispatch_event(wheel_event);
        }

        if (Container::focused_element) {
            Container::focused_element->on_input();
        }

        if (frames % (60 * 2) == 0) {
            String mb = String::from_double(ram_mb(), 2);
            mb.append(" MB");
            ram_usage_label->text = mb;
        }

        RayLib::BeginDrawing();
        RayLib::ClearBackground(Colors::BG.to_ray());

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
