#pragma once
#include <cstdlib>
#include <cstring>
#include <vector>
#include <stdio.h>

class String {
    public:
        String() {
            this->c_str = (char*)malloc(16);
            capacity = 16;
        }

        String(char* c_str) {
            this->c_str = c_str;
            capacity = strlen(c_str);
        }

        void append(char c) {
            if (capacity < (strlen(c_str) + 2)) {
                capacity *= 2;

                char* new_string = (char*)malloc(capacity);
                memcpy(new_string, c_str, capacity);
                free(c_str);

                c_str = new_string;
            }

            size_t len = strlen(c_str);
            c_str[len] = c;
            c_str[len + 1] = '\0';
        }

        std::vector<String> split(const char dilemeter) {
            std::vector<String> out;
            size_t substring_start = 0;

            size_t i = 0;
            while (true) {
                const char c = c_str[i];

                if (c == dilemeter || c == '\0') {
                    // fingers crossed
                    size_t new_len = i - substring_start;

                    char* new_string = (char*)malloc(new_len + 1);
                    memcpy((void*)new_string, c_str + substring_start, new_len);

                    new_string[new_len] = '\0';
                    substring_start = i + 1;
                    out.push_back(String(new_string));

                }

                if (c == '\0') break;
                i++;
            }

            return out;
        }

        String first_n(size_t n) {
            char* str = (char*)malloc(n + 1);
            memcpy((void*)str, c_str, n);

            str[n] = '\0';
            return String(str);
        }

        const char* as_c() {
            return c_str;
        }

    private:
        char* c_str = nullptr;
        size_t capacity = 0;
};