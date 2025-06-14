#include <sys/wait.h>
#include "LSPClient/LSPClient.h"

void LSPClient::open_pipes() {
    printf("Starting clangd...\n");

    ASSERT(pipe(to_lsp_pipe) != -1, "Unable to pipe to lsp");
    ASSERT(pipe(from_lsp_pipe) != -1, "Unable to pipe from lsp");

    lsp_pid = fork();

    if (lsp_pid == 0) {
        close(to_lsp_pipe[1]);  // write
        close(from_lsp_pipe[0]);  // read

        // in -> to_lsp
        dup2(to_lsp_pipe[0], STDIN_FILENO);
        close(to_lsp_pipe[0]);

        dup2(from_lsp_pipe[1], STDOUT_FILENO);
        close(from_lsp_pipe[1]);

        execl("/usr/bin/clangd", "/usr/bin/clangd", "--log=error", NULL);

        ASSERT_NOT_REACHED("execl failure");
        return;
    }

    close(to_lsp_pipe[0]); // read
    close(from_lsp_pipe[1]); // write
}

void LSPClient::shutdown() {
    printf("Say goodbye to LSP!\n");
    
    send_lsp_message(build_request("shutdown", 777, nullptr));
    await_lsp_response();
    send_lsp_message(build_request("exit", Optional<int>(), nullptr));

    close(to_lsp_pipe[1]);

    printf("Waiting for LSP to exit\n");
    int _status;
    waitpid(lsp_pid, &_status, 0);

    printf("Goodbye!\n");
    close(from_lsp_pipe[0]);
}

String LSPClient::build_request(String method, Optional<int> id, std::unique_ptr<JSONObject> params) {
    auto object = JSONObject();

    object.set("jsonrpc", "2.0");
    if (id) object.set("id", *id);
    object.set("method", method);
    if (params) object.set("params", std::move(params));

    return object.to_string();
}

void LSPClient::send_lsp_message(String payload) {
    size_t len = payload.length();

    String in = "Content-Length: ";
    in.append(String::from_int((int)len));
    in.append("\r\n\r\n");
    in.append(payload);

    printf("Payload: %s\n", payload.truncated().as_c());

    write(to_lsp_pipe[1], in.as_c(), in.length());
    // printf("Done writing\n");
}

void LSPClient::poll_lsp() {
    struct pollfd fd_array[1] = {{from_lsp_pipe[0], POLLIN, 0}};
    int poll_ret = poll(fd_array, 1, 0);
    ASSERT(poll_ret != -1, "Unable to poll");

    if (poll_ret == 0) return;
    if (!(fd_array[0].revents & POLLIN)) return;

    String body = await_lsp_response();
    process_lsp_response(body);
}

void LSPClient::process_lsp_response(String body) {
    // printf("%s\n", body.as_c());

    // I HATE THIS. FIX THIS SOON.
    auto _object = JSONParser(body).parse();
    auto object = _object->as<JSONObject>();

    // for (auto& pair : *(object->data)) {
    //     printf("%s\n", pair.first.as_c());
    // }

    auto method = object->get<JSONString>("method")->value;

    if (method == "textDocument/publishDiagnostics") {
        auto diagnostics = object->get<JSONObject>("params")->get<JSONArray>("diagnostics");

        diagnostic_messages.perform_on([&diagnostics](auto& messages) {
            for (auto& v : *(diagnostics->data)) {
                auto message = v->as<JSONObject>()->get<JSONString>("message")->value;
                messages.push_back(message.as_c());
                // printf("diag: %s\n", message.as_c());
            }
        });

    } else {
        printf("Unknown method '%s'\n", method.as_c());
    }
}

String LSPClient::await_lsp_response() {
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
        // printf("%s\n", line.as_c());
        
        auto header_line = line.split(": ");

        // Realistically this isn't impossible but let's hope it pretends to be
        ASSERT(header_line.size() == 2, "Ok who did that");

        headers[header_line[0]] = header_line[1];
    }

    int content_length = headers["Content-Length"].to_int();
    // printf("Content length: %d\n", content_length);

    String body = String::from_fd(from_lsp_pipe[0], content_length, 1024);
    // printf("Debug: %s\n", body.as_c());
    return body;
}

[[noreturn]] void LSPClient::lsp_thread() {
    printf("[lsp] hey from lsp thread\n");

    auto params = std::make_unique<JSONObject>();
    params->set_new<JSONNull>("processId");
    // params->set("rootPath", JSONNull::the());
    params->set("rootUri", "file:///home/susan/clui");

    auto client_info = params->set_new<JSONObject>("clientInfo");
    client_info->set("name", "claires lsp client");
    client_info->set("version", "0.000000001");

    auto capabilities = params->set_new<JSONObject>("capabilities");
    auto capabilities_window = capabilities->set_new<JSONObject>("window");
    capabilities_window->set_new<JSONBoolean>("workDoneProgress", true);


    send_lsp_message(build_request("initialize", 0, std::move(params)));
    auto response = await_lsp_response();
    // printf("%s\n", response.as_c());

    params = std::make_unique<JSONObject>();
    send_lsp_message(build_request("initialized", Optional<int>(), std::move(params)));

    params = std::make_unique<JSONObject>();
    auto text_document = params->set_new<JSONObject>("textDocument");
    text_document->set_new<JSONString>("uri", "file:///home/susan/clui/src/main.cpp");
    text_document->set_new<JSONString>("languageId", "cpp");
    text_document->set_new<JSONNumber>("version", 1);
    text_document->set_new<JSONString>("text", File("src/main.cpp").read());
    // TEXT

    send_lsp_message(build_request("textDocument/didOpen", Optional<int>(), std::move(params)));


    struct timespec sleep_time = { 0, 0 };
    sleep_time.tv_nsec = 10 * 1000 * 1000;

    while (true) {
        nanosleep(&sleep_time, NULL);
        poll_lsp();
        // auto response = await_lsp_response();
        // printf("%s\n", response.as_c());
    }
}