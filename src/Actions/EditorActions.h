#pragma once

#include "UI/Terminal/Terminal.h"
#include "UI/TabContainer/TabContainer.h"

class EditorActions {
public:
    virtual ~EditorActions() = 0;

    static void open_file_in_new_tab(String path);
    static void register_primary_tab_container(TabContainer* container);

private:
    static TabContainer* primary_tab_container;
};

class Action {
public:
    virtual ~Action() = default;

    virtual void execute() { }
};

class CommandAction : public Action {
public:
    String command;

    CommandAction(String command) : command(command) { }

    void execute() override {
        Terminal* term = Terminal::task_terminal();
        term->send_command(command);
    }
};
