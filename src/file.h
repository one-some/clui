#pragma once

#include <cstddef>
#include <cstdio>
#include "string.h"
#include "assert.h"

class File {
    public:
        // XXX
        File(const char* path) {
            this->path = path;
        }

        String read() {
            fp = fopen(path, "rb");
            ASSERT(fp != NULL, "Couldn't open file");

            fseek(fp, 0, SEEK_END);
            length = ftell(fp);
            fseek(fp, 0, SEEK_SET);

            char* buf = (char*)malloc(length + 1);

            size_t amount_read = fread(buf, 1, length, fp);
            ASSERT(amount_read == length, "Didn't read all..?");

            buf[length] = '\0';
            fclose(fp);

            return String(buf);
        }
    
    private:
        const char* path = "";
        FILE* fp = nullptr;
        size_t length = 0;
};