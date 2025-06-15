#include "Actions/EditorActions.h"
#include "UI/TextEdit/TextEdit.h"
#include "LSPClient/LSPClient.h"
#include "Claire/Path.h"

TabContainer* EditorActions::primary_tab_container = nullptr;

void EditorActions::open_file_in_new_tab(String path) {
    if (!primary_tab_container) return;
    path = Path::global(path);

    if (File(path).is_probably_binary()) {
        // We do a lot of shite like this which causes us to open like 5 file handles for each file (hopefully we close them...)
        printf("Apparently we are 'probably binary'");
        return;
    }

    printf("Opening '%s'\n", path.as_c());

    auto editor = std::make_unique<TextEdit>(path);
    editor->debug_name = "editor";
    editor->size->strategy_y = SizeStrategy::EXPAND_TO_FILL;

    auto tab = primary_tab_container->add_tab(path.as_c(), std::move(editor), true);
    primary_tab_container->focus_tab(tab);

    LSPClient::the().file_did_open(path);
}

void EditorActions::register_primary_tab_container(TabContainer* container) {
    primary_tab_container = container;
}
