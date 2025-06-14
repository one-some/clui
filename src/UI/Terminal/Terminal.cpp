#include <unistd.h>
#include <pty.h>
#include <poll.h>

#include "UI/Terminal/Terminal.h"
#include "color.h"

std::vector<Terminal*> Terminal::terminals = {};

void Terminal::draw_self() {
    sync_pty();
    draw_text();
}

void Terminal::draw_text() {
    Vector2 pos = get_draw_position();
    std::vector<String> lines = text.split('\n');

    for (size_t i=0; i<lines.size();i++) {
        RayLib::DrawTextEx(
            font,
            lines[i].as_c(),
            { (float)pos.x + 4, (float)pos.y + (font_size_px * i) },
            (float)font_size_px,
            0,
            Colors::FG.to_ray()
        );
    }

    caret_blink_timer++;

    // Sleepy. I think this can be made more beautiful.
    bool caret_visible = true;
    if (caret_blink_timer > CARET_BLINK_DURATION) {
        caret_blink_timer = 0;
    } else if (caret_blink_timer > CARET_BLINK_DURATION / 2) {
        caret_visible = false;
    }

    if (caret_visible && is_focused()) {
        // TODO: Wht tf is this every frame whyyyyyyyyyy
        auto mono_size = Vector2::from_ray(RayLib::MeasureTextEx(
            font,
            "s",
            font_size_px,
            0
        ));

        RayLib::DrawRectangle(
            pos.x + (mono_size.x * lines.back().length()) + 4,
            pos.y + (mono_size.y * (lines.size() - 1)),
            mono_size.x,
            mono_size.y,
            Colors::FG.to_ray()
        );
    }
}

void Terminal::init_terminal() {
    printf("Initing terminal...\n");

    int slave_fd;

    ASSERT(
        openpty(&master_pty_fd, &slave_fd, NULL, NULL, NULL)
        == 0,
        "Failure in openpty()"
    );

    if (fork() == 0) {
        // Child
        close(master_pty_fd);

        setsid();
        ioctl(slave_fd, TIOCSCTTY, 1);

        struct termios terminal_attrs;

        ASSERT(tcgetattr(slave_fd, &terminal_attrs) != -1, "termattr get error");

        terminal_attrs.c_lflag |= ICANON;
        terminal_attrs.c_lflag |= ECHO;
        terminal_attrs.c_lflag |= ISIG;
        terminal_attrs.c_oflag |= OPOST;
        terminal_attrs.c_iflag |= ICRNL;

        terminal_attrs.c_cc[VERASE] = '\b';

        ASSERT(tcsetattr(slave_fd, TCSANOW, &terminal_attrs) != -1, "termattr write error");

        dup2(slave_fd, 0);  // STDIN
        dup2(slave_fd, 1);  // STDOUT
        dup2(slave_fd, 2);  // STDERR

        close(slave_fd);

        setenv("TERM", "dumb", 1);
        execl("/bin/bash", "/bin/bash", "--norc", NULL);
        return;
    }

    // Parent
    close(slave_fd);
}

void Terminal::sync_pty() {
    String out;

    struct pollfd fd_array[1] = {{master_pty_fd, POLLIN, 0}};

    char* buf = (char*)calloc(1024, 1);

    while (true) {
        // Check if we can read without blocking for a bajillion years.....
        int poll_ret = poll(fd_array, 1, 0);
        ASSERT(poll_ret != -1, "Unable to poll");

        if (poll_ret == 0) break;
        if (!(fd_array[0].revents & POLLIN)) break;

        // Ok probably safe lol
        read(master_pty_fd, buf, 1024 - 1);
        out.append(buf);

    }

    free(buf);

    if (out.as_c()[0]) {
        for (size_t i = 0; i < strlen(out.as_c()); i++) {
            char c = out.as_c()[i];

            if (c == '\b') {
                text.remove(strlen(text.as_c()) - 1);
                continue;
            }

            if (c == '\r') continue;

            if (c == '\n') {
                caret_pos = 0;
            } else {
                caret_pos++;
            }

            text.add_char(c);
        }
    }
    return;
}

void Terminal::on_input() {
    char c = '\0';
    String keys;

    while ((c = (char)RayLib::GetCharPressed())) {
        keys.add_char(c);
    }

    if (RayLib::IsKeyTyped(RayLib::KEY_ENTER))      { keys.add_char('\n'); }
    if (RayLib::IsKeyTyped(RayLib::KEY_BACKSPACE))  { keys.add_char('\b'); }
    if (RayLib::IsKeyTyped(RayLib::KEY_TAB))        { keys.add_char('\t'); }

    if (RayLib::IsKeyDown(RayLib::KEY_LEFT_CONTROL)) {
        if (RayLib::IsKeyTyped(RayLib::KEY_C)) keys.add_char('\x03');
    }

    write(master_pty_fd, keys.as_c(), keys.length());
}
