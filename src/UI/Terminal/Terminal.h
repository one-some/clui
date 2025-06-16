#pragma once
#include <vector>
#include <unistd.h>

#include "fontglobal.h"
#include "Claire/Assert.h"
#include "Claire/String.h"
#include "UI/Container/Container.h"

class Terminal : public Container {
public:
    static const int32_t CARET_BLINK_DURATION = 530 / 8;

    String text;
    int master_pty_fd = -1;

    RayLib::Font font = Font::the();
    int32_t font_size_px = 16;
    int32_t caret_blink_timer = 0;
    uint32_t caret_pos = 0;

    Terminal() {
        terminals.push_back(this);

        allow_scroll = true;
        init_terminal();
    }

    ~Terminal() {
        terminals.erase(std::remove(terminals.begin(), terminals.end(), this), terminals.end());
    }

    virtual void draw_self() override;
    void draw_text();
    void init_terminal();

    void send_command(String command) {
        write(master_pty_fd, "\x03", 1);
        // HACK!!!!! MAJOR HACK!!!!!!! TODO: QUEUE UP THESE THINGS AND FLUSH
        usleep(15000);
        command = command + "\n";
        write(master_pty_fd, command.as_c(), command.length());
    }

    static Terminal* task_terminal() {
        // Stupid hack

        ASSERT(terminals.size(), "No terminals");
        return terminals[0];
    }

private:
    static std::vector<Terminal*> terminals;

    void on_input() override;
    void sync_pty();
};
