#include <unistd.h>
#include <pty.h>
#include <poll.h>

#include "terminal.h"
#include "color.h"

void Terminal::draw_self() {
    sync_pty();
    handle_input();
    draw_text();
}

void Terminal::draw_text() {
    Vector2 pos = position->get_global();
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

        dup2(slave_fd, 0);  // STDIN
        dup2(slave_fd, 1);  // STDOUT
        dup2(slave_fd, 2);  // STDERR

        close(slave_fd);

        execl("/bin/sh", "");
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
        read(master_pty_fd, buf, 1024);
        out.append(buf);

    }

    free(buf);

    if (out.as_c()[0]) {
        printf("%s\n", out.as_c());
        text.append(out.as_c());
    }
    return;
}

void Terminal::handle_input() {
    char c = '\0';
    String keys;

    while ((c = RayLib::GetCharPressed())) {
        keys.add_char(c);
    }

    if (RayLib::IsKeyPressed(RayLib::KEY_ENTER)) {
        keys.add_char('\n');
    }

    write(master_pty_fd, keys.as_c(), strlen(keys.as_c()));
}