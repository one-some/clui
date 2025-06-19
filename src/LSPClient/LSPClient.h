#pragma once

#ifdef __linux__
    #include <pty.h>
#elif _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#endif

#include <poll.h>

#include "Claire/File.h"
#include "Claire/Thread.h"
#include "Claire/String.h"
#include "Claire/JSON/JSON.h"
#include "Claire/FamousResource.h"

class LSPClient {
private:
    LSPClient() {
        open_pipes();
    }

    ~LSPClient() {
        shutdown();
    }

    void shutdown();

public:
#ifdef __linux__
    int to_lsp_pipe[2];
    int from_lsp_pipe[2];
    pid_t lsp_pid;
#elif _WIN32
    HANDLE to_lsp_write = NULL;
    HANDLE from_lsp_read = NULL;
#endif
    FamousResource<std::vector<String>, FamousLocking::Mutex> diagnostic_messages;

    static LSPClient& the() {
        static LSPClient real_deal;
        return real_deal;
    }

    void open_pipes();

    void init() {
        auto thread = Thread([this] {
            lsp_thread();
        });
        thread.start();
        thread.daemonify();
    }

    String build_request(String method, Optional<int> id, std::unique_ptr<JSONObject> params);
    void send_lsp_message(const String payload);

    void poll_lsp();
    void process_lsp_response(String body);

    String await_lsp_response();
    void file_did_open(String path);

    [[noreturn]] void lsp_thread();
};
