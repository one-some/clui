#pragma once

#include <cstddef>
#include <cstdio>
#include "Claire/String.h"
#include "Claire/Assert.h"

class File {
    // FIXME: It look not so good./.....
    public:
        File(const char* path) {
            this->path = path;
        }

        bool is_probably_binary() {
            // Kinda lame heuristic but lazy...
            size_t null_index = read().length();
            size_t length = get_length();

            printf("%s: %li vs %li\n", path, null_index, length);

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
            size_t len = get_length();
            auto fp = get_read_pointer();

            char* buf = (char*)malloc(len + 1);
            size_t amount_read = fread(buf, 1, len, fp);
            ASSERT(amount_read == len, "Didn't read all..?");

            buf[len] = '\0';
            fclose(fp);

            return String::move_from(buf);
        }

        void write(String content) {
            FILE* fp = fopen(path, "w");
            ASSERT(fp != NULL, "Couldn't open file");

            fprintf(fp, "%s", content.as_c());

            fclose(fp);
        }
    
    private:
        const char* path = "";

        FILE* get_read_pointer() {
            // Remember you gotta close it...
            FILE* fp = fopen(path, "rb");
            ASSERT(fp != NULL, "Couldn't open file");

            return fp;
        }
};