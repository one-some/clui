// TODO: https://include-what-you-use.org/

#include "LSPClient/LSPClient.h"
#include <sys/wait.h>
#include "Claire/Time.h"

void LSPClient::open_pipes() {
    printf("Starting clangd...\n");

#ifdef __linux__
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
#elif _WIN32
    HANDLE to_lsp_read = NULL;
    HANDLE from_lsp_write = NULL;

    // U gotta be kidding me
    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;

    ASSERT(CreatePipe(&to_lsp_read, &to_lsp_write, &sa, 0), "Unable to pipe to lsp");
    ASSERT(CreatePipe(&from_lsp_read, &from_lsp_write, &sa, 0), "Unable to pipe from lsp");

    STARTUPINFO si;
    ZeroMemory(&si, sizeof(STARTUPINFO));
    sa.cb = sizeof(STARTUPINFO);
    si.hStdError = GetStdHandle(STD_ERROR_HANDLE);
    si.hStdOutput = from_lsp_write;
    si.hStdInput = to_lsp_read;
    si.dwFlags |= STARTF_USESTDHANDLES;

    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
    char exec = "clang.exe";

    BOOL success = CreateProcess(
        NULL,
        exec,
        NULL,
        NULL,
        TRUE,
        0,
        NULL,
        NULL,
        &si,
        &pi
    );

    ASSERT(success, "Can't create process");
    CloseHandle(from_lsp_write);
    CloseHandle(to_lsp_read);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

#endif
}

void LSPClient::shutdown() {
    printf("Say goodbye to LSP!\n");
    
    send_lsp_message("shutdown", 777, nullptr);
    await_lsp_response();
    send_lsp_message("exit", Optional<int>(), nullptr);

    close(to_lsp_pipe[1]);

    printf("Waiting for LSP to exit\n");
    int _status;
    waitpid(lsp_pid, &_status, 0);

    printf("Goodbye!\n");
    close(from_lsp_pipe[0]);
}

void LSPClient::send_lsp_message(String method, Optional<int> id, std::unique_ptr<JSONObject> params) {
    auto object = JSONObject();

    object.set("jsonrpc", "2.0");
    if (id) {
        object.set("id", *id);
        request_id_methods[*id] = method;
    }
    object.set("method", method);
    if (params) object.set("params", std::move(params));

    String payload = object.to_string();
    int len = payload.length();

    String in = "Content-Length: ";
    in.append(String::from_int(len));
    in.append("\r\n\r\n");
    in.append(payload);

    printf("Payload: %s\n", payload.truncated().as_c());
    // printf("Payload: %s\n", payload.as_c());

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
    printf("%s\n", body.as_c());

    // I HATE THIS. FIX THIS SOON.
    auto _object = JSONParser(body).parse();
    auto object = _object->as<JSONObject>();

    // for (auto& pair : *(object->data)) {
    //     printf("%s\n", pair.first.as_c());
    // }

    String method;
    if (object->has("id")) {
        int id = object->get<JSONNumber>("id")->value;
        ASSERT(request_id_methods.contains(id), "We are making some bold assumptions that are actually bogus.");

        method = request_id_methods[id];
        request_id_methods.erase(id);
    } else {
        method = object->get<JSONString>("method")->value;
    }

    if (method == "textDocument/publishDiagnostics") {
        printf("%s\n", object->to_string().as_c());
        auto params = object->get<JSONObject>("params");

        auto diagnostics = params->get<JSONArray>("diagnostics");
        auto uri = params->get<JSONString>("uri")->value;

        diagnostic_messages.perform_on([&diagnostics, uri](auto& messages) {
            for (auto& v : *(diagnostics->data)) {
                auto v_obj = v->as<JSONObject>();
                auto message = v_obj->get<JSONString>("message")->value;

                auto range_obj = v_obj->get<JSONObject>("range");
                auto range_start_obj = range_obj->get<JSONObject>("start");
                auto range_end_obj = range_obj->get<JSONObject>("end");

                TextCoordinate range_start = {
                    range_start_obj->get<JSONNumber>("line")->value,
                    range_start_obj->get<JSONNumber>("character")->value,
                };

                TextCoordinate range_end = {
                    range_end_obj->get<JSONNumber>("line")->value,
                    range_end_obj->get<JSONNumber>("character")->value,
                };

                messages.push_back(EditorDiagnostic(
                    message,
                    uri,
                    range_start,
                    range_end
                ));
                // printf("diag: %s\n", message.as_c());
            }
        });
    } else if (method == "textDocument/hover") {
        // If result doesn't exist something has gone wrong... oops!
        auto result_ambigious = object->get("result");

        // Not the best but idk any other way to do it
        if (result_ambigious->is<JSONNull>()) return;

        auto result = result_ambigious->as<JSONObject>();
        auto contents = result->get<JSONObject>("contents");

        String type = contents->get<JSONString>("kind")->value;
        ASSERT(type == "plaintext", "hover: Ok idk what that type '%s' is", type.as_c());

        String value = contents->get<JSONString>("value")->value;
        printf("\nXXX\n%s\n\n", value.as_c());
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


    send_lsp_message("initialize", 0, std::move(params));
    auto response = await_lsp_response();
    // printf("%s\n", response.as_c());

    params = std::make_unique<JSONObject>();
    send_lsp_message("initialized", Optional<int>(), std::move(params));

    while (true) {
        Time::sleep_ms(10);
        poll_lsp();
    }
}

void LSPClient::file_did_open(String path) {
    auto file_type = path.split(".").back();

    auto params = std::make_unique<JSONObject>();
    auto text_document = params->set_new<JSONObject>("textDocument");
    text_document->set_new<JSONString>("uri", "file://" + path);
    text_document->set_new<JSONString>("languageId", file_type);
    text_document->set_new<JSONNumber>("version", 1);
    text_document->set_new<JSONString>("text", File(path).read());

    send_lsp_message("textDocument/didOpen", Optional<int>(), std::move(params));
}

void LSPClient::file_did_hover(String path, int line, int col) {
    auto params = std::make_unique<JSONObject>();
    auto doc = params->set_new<JSONObject>("textDocument");
    doc->set_new<JSONString>("uri", "file://" + path);

    auto position = params->set_new<JSONObject>("position");
    position->set_new<JSONNumber>("line", line);
    position->set_new<JSONNumber>("character", line);

    send_lsp_message("textDocument/hover", Optional<int>(99), std::move(params));
}
