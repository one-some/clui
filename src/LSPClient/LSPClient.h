#pragma once

#ifdef __linux__
    #include <pty.h>
    #include <poll.h>
#elif _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#endif

#include "Claire/File.h"
#include "Claire/Thread.h"
#include "Claire/String.h"
#include "Claire/JSON/JSON.h"
#include "Claire/FamousResource.h"
#include "UI/TextEdit/EditorDiagnostic.h"

struct PendingRequest {
    String method;
    std::function<void(const JSONObject&)> callback;
};

class LSPClient {
private:
    LSPClient() {
        open_pipes();
    }

    ~LSPClient() {
        shutdown();
    }

    void shutdown();

    static int request_id;

    static int get_new_request_id() {
        return request_id++;
    }

    std::map<int, PendingRequest> pending_requests = {};

public:
#ifdef __linux__
    int to_lsp_pipe[2];
    int from_lsp_pipe[2];
    pid_t lsp_pid;
#elif _WIN32
    HANDLE to_lsp_write = NULL;
    HANDLE from_lsp_read = NULL;
#endif
    FamousResource<std::vector<EditorDiagnostic>, FamousLocking::Mutex> diagnostic_messages;

    static LSPClient& the() {
        static LSPClient real_deal;
        return real_deal;
    }

    std::vector<EditorDiagnostic> diagnostics_for(String path) {
        std::vector<EditorDiagnostic> out;

        // Is this the lazy way? Yaaa
        path = "file://" + path;

        for (auto diag : *(diagnostic_messages.get())) {
            if (path != diag.file_path) continue;
            out.push_back(std::move(diag));
        }

        return out;
    }

    void open_pipes();

    void init() {
        auto thread = Thread([this] {
            lsp_thread();
        });
        thread.start();
        thread.daemonify();
    }

    void send_lsp_message(
        String method,
        Optional<int> id,
        std::unique_ptr<JSONObject> params,
        std::function<void(const JSONObject&)> callback
    );

    void poll_lsp();
    void process_lsp_response(String body);

    String await_lsp_response();

    void file_did_open(String path);
    void file_did_hover(
        String path,
        int line,
        int col,
        std::function<void(const JSONObject&)> callback
    );

    [[noreturn]] void lsp_thread();
};
