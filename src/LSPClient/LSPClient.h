#include <pty.h>
#include <poll.h>

#include "Claire/String.h"
#include "Claire/JSON/JSON.h"

class LSPClient {
public:
    int to_lsp_pipe[2];
    int from_lsp_pipe[2];

    LSPClient() {
        open_pipes();
    }

    void open_pipes() {
        printf("Starting clangd...\n");

        ASSERT(pipe(to_lsp_pipe) != -1, "Unable to pipe to lsp");
        ASSERT(pipe(from_lsp_pipe) != -1, "Unable to pipe from lsp");

        if (fork() == 0) {
            close(to_lsp_pipe[1]);  // write
            close(from_lsp_pipe[0]);  // read

            // in -> to_lsp
            dup2(to_lsp_pipe[0], STDIN_FILENO);
            close(to_lsp_pipe[0]);

            dup2(from_lsp_pipe[1], STDOUT_FILENO);
            close(from_lsp_pipe[1]);

            execl("/usr/bin/clangd", "/usr/bin/clangd", NULL);

            ASSERT_NOT_REACHED("execl failure");
            return;
        }

        close(to_lsp_pipe[0]); // read
        close(from_lsp_pipe[1]); // write
    }

    String build_request() {
        auto object = new JSONObject();
        object->set("jsonrpc", "2.0");
        object->set("id", 1);
        object->set("method", "initialize");
        
        auto params = object->set_new<JSONObject>("params");

        params->set_new<JSONNull>("processId");
        // params->set("rootPath", JSONNull::the());
        params->set("rootUri", "file:///home/susan/clui");

        auto client_info = params->set_new<JSONObject>("clientInfo");
        client_info->set("name", "claires lsp client");
        client_info->set("version", "0.000000001");

        auto capabilities = params->set_new<JSONObject>("capabilities");
        // TODO: Be able!

        return object->to_string();
    }

    String await_lsp_response(String payload) {
        size_t len = payload.length();

        String in = "Content-Length: ";
        in.append(String::from_int(len));
        in.append("\r\n\r\n");
        in.append(payload);

        printf("Payload: %s\n", payload.as_c());

        write(to_lsp_pipe[1], in.as_c(), in.length());
        printf("Done writing\n");

        // Now we wait....

        String header_chunk;
        while (true) {
            // WHAT'S UP WITH THIS TERRIBLY UNOPTIMIZED THING!?!?! WOW!!!!!!
            char c; 
            ssize_t bytes_read = read(from_lsp_pipe[0], &c, 1);

            ASSERT(bytes_read != EOF, "Oook we just hit eof in header ROFL");
            header_chunk.add_char(c);

            // After this we're in bodyland...ominous...
            if (header_chunk.last_n(4) == "\r\n\r\n") {
                break;
            }
        }

        std::map<String, String> headers;
        // TODO: Make a .strip() lol
        for (auto& line : header_chunk.split("\r\n")) {
            if (!line.length()) continue;
            printf("%s\n", line.as_c());
            
            auto header_line = line.split(": ");

            // Realistically this isn't impossible but let's hope it pretends to be
            ASSERT(header_line.size() == 2, "Ok who did that");

            headers[header_line[0]] = header_line[1];
        }

        int content_length = headers["Content-Length"].to_int();
        printf("Content length: %d\n", content_length);

        String packet = String::from_fd(from_lsp_pipe[0], content_length, 1024);
        printf("BEGIN%sEND\n", packet.as_c());

        // I HATE THIS. FIX THIS SOON.
        auto _object = JSONParser(packet).parse();
        auto object = _object->as<JSONObject>();

        for (auto& pair : *(object->data)) {
            printf("%s\n", pair.first.as_c());
        }



        // TODO FIXME
        return packet;
    }
};