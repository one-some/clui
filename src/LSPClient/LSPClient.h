#include <pty.h>
#include <poll.h>

#include "Claire/String.h"
#include "Claire/JSON/JSON.h"

class LSPClient {
public:
    int master_pty_fd = -1;

    LSPClient() {
        init_pty();
    }

    void init_pty() {
        printf("Starting clangd...\n");

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

            // setenv("TERM", "dumb", 1);
            execl("/usr/bin/clangd", "/usr/bin/clangd", NULL);
            return;
        }

        // Parent
        close(slave_fd);
    }

    String build_request() {
        auto object = new JSONObject();
        object->set("jsonrpc", "2.0");
        object->set("id", 1);
        object->set("method", "initialize");
        
        auto params = new JSONObject();
        object->set("params", params);

        params->set("processId", JSONNull::the());
        // params->set("rootPath", JSONNull::the());
        params->set("rootUri", "file:///home/susan/clui");

        auto client_info = new JSONObject();
        params->set("clientInfo", client_info);
        client_info->set("name", "claires lsp client");
        client_info->set("version", "0.000000001");

        auto capabilities = new JSONObject();
        params->set("capabilities", capabilities);
        // TODO: Be able!

        return object->to_string();
    }

    String await_lsp_response(String payload) {
        size_t len = payload.length();

        String in = "Content-Length: ";
        in.append(String::from_int(len));
        in.append("\r\nContent-Type: application/json; charset=utf-8\r\n\r\n");
        in.append(payload);

        printf("Payload: %s\n", payload.as_c());

        write(master_pty_fd, in.as_c(), in.length());
        printf("Done writing\n");

        // Now we wait....

        String header_chunk;
        while (true) {
            // WHAT'S UP WITH THIS TERRIBLY UNOPTIMIZED THING!?!?! WOW!!!!!!
            char c; 
            read(master_pty_fd, &c, 1);

            ASSERT(c != EOF, "Oook we just hit eof in header ROFL");
            header_chunk.add_char(c);

            // After this we're in bodyland...ominous...
            if (header_chunk.last_n(4) == "\r\n\r\n") {
                break;
            }
        }

        std::map<String, String> headers;
        // TODO: Make .split() accept a string so we dont need these \r hax
        // TODO: Also make a .strip() lol
        for (auto& line : header_chunk.split('\n')) {
            if (line == "\r") continue;
            if (!line.length()) continue;
            
            // SUPERTODO: Ok this is seriously getting annoying with the one char dilemeter
            auto header_line = line.split(':');

            // Realistically this isn't impossible but let's hope it pretends to be
            ASSERT(header_line.size() == 2, "Ok who did that");

            headers[header_line[0]] = header_line[1].slice(1, header_line[1].length());
        }

        for (auto& pair : headers) { 
            // FIXMENEXTTIME: I think the 212 (pair.second) has a null character...........................shiiiiiiiiteeeeeeeeeeeeeeeeeeee.......................ok goodnight 2:22AM
            printf("'%s': '%s'\n", pair.first.as_c(), pair.second.as_c());
        }

        // printf("Out: %s\n", out.as_c());

        // TODO FIXME
        return header_chunk;
    }
};