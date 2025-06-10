#pragma once

#include <cstddef>
#include <cstdio>
#include "Claire/String.h"
#include "Claire/Assert.h"

class File {
    // FIXME: It look not so good./.....
    public:
        File(String path) : path(path) { }

        bool is_probably_binary() {
            // Kinda lame heuristic but lazy...
            size_t null_index = read().length();
            size_t length = get_length();

            return length != null_index;
        }

        size_t get_length() {
            auto fp = get_read_pointer();

            fseek(fp, 0, SEEK_END);
            size_t length = ftell(fp);
            fclose(fp);

            return length;
        }

        String read() {
            // size_t len = get_length();
            // auto fp = get_read_pointer();

            // char* buf = (char*)malloc(len + 1);
            // size_t amount_read = fread(buf, 1, len, fp);
            // ASSERT(amount_read == len, "Didn't read all..?");

            // printf("Reading %ld from %s\n", amount_read, path.as_c());
            auto fp = get_read_pointer();

            String out;
            char* buf = (char*)malloc(1024 + 1);
            while (true) {
                size_t read_count = fread(buf, 1, 1024, fp);
                buf[read_count] = '\0';
                out.append(buf);
                if (read_count <= 0) break;
            }

            fclose(fp);
            free(buf);

            return out;
        }

        void write(String content) {
            FILE* fp = fopen(path.as_c(), "w");
            ASSERT(fp != NULL, "Couldn't open file");

            fprintf(fp, "%s", content.as_c());

            fclose(fp);
        }

        String get_path() const { return path; }
    
    private:
        const String path;

        FILE* get_read_pointer() {
            // Remember you gotta close it...
            FILE* fp = fopen(path.as_c(), "rb");
            ASSERT(fp != NULL, "Couldn't open file");

            return fp;
        }
};