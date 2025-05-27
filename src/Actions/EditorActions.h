#pragma once

#include "UI/TabContainer/TabContainer.h"

class EditorActions {
public:
    virtual ~EditorActions() = 0;

    static void open_file_in_new_tab(const char* path);
    static void register_primary_tab_container(TabContainer* container);

private:
    static TabContainer* primary_tab_container;
};