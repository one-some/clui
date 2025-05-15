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

        String read() {
            FILE* fp = fopen(path, "rb");
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

        void write(String content) {
            FILE* fp = fopen(path, "rb");
            ASSERT(fp != NULL, "Couldn't open file");

            fprintf(fp, "%s", content.as_c());

            fclose(fp);
        }
    
    private:
        const char* path = "";
        size_t length = 0;
};