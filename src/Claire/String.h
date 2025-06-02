#pragma once
#include <cmath>
#include <cstdlib>
#include <cstddef>

#include "Claire/Assert.h"
#include "Claire/Math.h"
#include <vector>
#include <stdio.h>
#include <cstring>

class String {
    public:
        String() {
            c_str = (char*)calloc(16, 1);
            capacity = 16;
        }

        String(const char* in_c_str) {
            capacity = strlen(in_c_str) + 1;
            c_str = (char*) malloc(capacity);
            strcpy(c_str, in_c_str);
            c_str[capacity - 1] = '\0';
        }

        static String from_double(double val) {
            int len = snprintf(NULL, 0, "%f", val);
            char* buf = (char*) malloc(len + 1);
            snprintf(buf, len + 1, "%f", val);
            return String::move_from(buf);
        }

        static String from_int(int val) {
            int len = snprintf(NULL, 0, "%d", val);
            char* buf = (char*) malloc(len + 1);
            snprintf(buf, len + 1, "%d", val);
            return String::move_from(buf);
        }

        static String from_number_klutz(double val) {
            if (val == std::floor(val)) return from_int((int) val);
            return from_double(val);
        }

        static char* copy_c_str(const char* in_c_str) {
            char* c_str = (char*) malloc(strlen(in_c_str + 1));
            strcpy(c_str, in_c_str);
            c_str[strlen(in_c_str)] = '\0';
            return c_str;
        }

        static String move_from(char* in_c_str) {
            String out;
            out.capacity = strlen(in_c_str) + 1;
            out.c_str = (char*)std::move(in_c_str);
            return out;
        }

        ~String() {
            free(c_str);
        }
        
        const char operator[](size_t index) {
            return c_str[index];
        }

        String(const String& that) : c_str(nullptr), capacity(0) {
            if (!that.c_str) return;

            capacity = strlen(that.c_str) + 1;
            c_str = (char*) calloc(capacity, 1);
            strcpy(c_str, that.c_str);
        }

        String& operator=(const String& that) {
            if (this == &that) return *this;

            free(c_str);
            capacity = that.capacity;
            c_str = (char*) calloc(capacity, 1);
            strcpy(c_str, that.c_str);
            return *this;
        }

        String& operator=(String&& other) noexcept {
            if (this == &other) return *this;

            free(c_str);
            c_str = other.c_str;
            capacity = other.capacity;

            other.c_str = nullptr;
            other.capacity = 0;
            return *this;
        }

        bool operator==(const String& that) const {
            return strcmp(c_str, that.c_str) == 0;
        }

        bool operator==(const char* that) const {
            return strcmp(c_str, that) == 0;
        }

        bool operator<(const String& that) const {
            size_t this_len = length();
            size_t that_len = that.length();
            size_t min_len = min(this_len, that_len);

            int cmp_out = std::strncmp(c_str, that.c_str, min_len);
            if (cmp_out == 0) return this_len < that_len;
            return cmp_out < 0;
        }
        
        constexpr static bool is_number(const char c) {
            return c >= '0' && c <= '9';
        }

        constexpr static u_int64_t hash(const char* str) {
            // djb2 by Dan Bernstein
            u_int64_t hash = 5381;
            int c = 0;

            while ((c = *str++)) {
                hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
            }

            return hash;
        }

        u_int64_t hash() const {
            return String::hash(c_str);
        
        }
        
        size_t length() const {
            return strlen(c_str);
        }

        void add_char(char c) {
            expand_for(1);

            size_t len = strlen(c_str);
            c_str[len] = c;
            c_str[len + 1] = '\0';
        }

        void append(String text) {
            append(text.c_str);
        }

        void append(const char* text) {
            if (!text[0]) return;

            expand_for(strlen(text));
            memmove(c_str + strlen(c_str), text, strlen(text));
            c_str[strlen(c_str)] = '\0';
        }

        void insert(char c, size_t index) {
            expand_for(1);

            memmove(c_str + index + 1, c_str + index, strlen(c_str) + 1 - index);
            c_str[index] = c;
        }

        void remove(size_t index) {
            // Just keep it current size lol
            memmove(c_str + index, c_str + index + 1, strlen(c_str) - index);
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
                    out.push_back(String::move_from(new_string));

                }

                if (c == '\0') break;
                i++;
            }

            return out;
        }

        String first_n(size_t n) const {
            char* str = (char*)calloc(n + 1, 1);

            memcpy((void*)str, c_str, n);
            str[n] = '\0';

            return String(str);
        }

        String last_n(size_t n) {
            size_t len = length();
            if (!len) return "";

            size_t start = (n > len) ? 0 : len - n;
            return slice(start, len);
        }

        String slice(size_t start, size_t end) {
            if (start == end) return String("");

            ASSERT(start < end, "Be orderly!");
            char* str = (char*)calloc(end - start + 1, 1);

            memcpy(str,  c_str + start, end - start);
            str[end - start] = '\0';

            return String(str);
        }

        const char* as_c() const {
            return c_str;
        }
        
        float as_float() {
            // TODO: Error check or DIE!!!!
            char* end;
            return strtof(c_str, &end);
        }

    private:
        char* c_str = nullptr;
        size_t capacity = 0;

        void expand_for(size_t extra_chars) {
            if (capacity >= (strlen(c_str) + 1 + extra_chars)) return;

            size_t old_capacity = capacity;
            capacity = (capacity + extra_chars) * 2;

            char* new_string = (char*)calloc(capacity, 1);
            memcpy(new_string, c_str, old_capacity);
            free(c_str);

            c_str = new_string;
        }
};