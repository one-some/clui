#pragma once
#include <cmath>
#include <cstdlib>
#include <cstddef>
#include <unistd.h>

#include "Claire/Optional.h"
#include "Claire/Assert.h"
#include "Claire/Math.h"
#include <vector>
#include <stdio.h>
#include <cstring>

class String {
public:
    String() {
        capacity = 16;
        c_str = (char*)calloc(capacity, 1);
    }

    String(char c) {
        capacity = 4;
        c_str = (char*)malloc(capacity);
        c_str[0] = c;
        c_str[1] = '\0';
    }

    String(const char* in_c_str) {
        capacity = strlen(in_c_str) + 1;
        c_str = (char*) malloc(capacity);
        strcpy(c_str, in_c_str);
        c_str[capacity - 1] = '\0';
    }

    static String from_double(double val, int round_places = -1) {
        int len = snprintf(NULL, 0, "%f", val);
        char* buf = (char*) malloc(len + 1);
        snprintf(buf, len + 1, "%f", val);

        auto out = String::move_from(buf);

        if (round_places > 0) {
            auto dot_pos = out.find(".");
            out = out.slice(0, (*dot_pos) + round_places + 1);
        }

        return out;
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

    static String move_from(char* in_c_str) {
        if (!in_c_str) return String("");

        // TODO: Make "empty" constructor
        String out;
        free(out.c_str);

        out.capacity = strlen(in_c_str) + 1;
        out.c_str = in_c_str;
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
    
    constexpr static bool is_number(const char c) { return c >= '0' && c <= '9'; }
    constexpr static bool is_lowercase(const char c) { return c >= 'a' && c <= 'z'; }
    constexpr static bool is_uppercase(const char c) { return c >= 'A' && c <= 'Z'; }
    constexpr static bool is_letter(const char c) { return is_lowercase(c) || is_uppercase(c); }

    bool is_number() const {
        if (!length()) return false;

        for (size_t i = 0; i < length(); i++) {
            if (!String::is_number(c_str[i])) return false;
        }

        return true;
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

    void append(const String& text) { append(text.c_str); }

    void append(const char* text) {
        if (!text || !text[0]) return;

        size_t old_len = length();
        size_t input_len = strlen(text);
        expand_for(input_len);

        memmove(c_str + old_len, text, input_len);
        c_str[old_len + input_len] = '\0';
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

    std::vector<String> split(const String& delimiter) {
        std::vector<String> out;

        if (!delimiter.length()) {
            // Otherwise will be empty
            if (!length()) out.push_back("");

            // TODO: Iterator over chars...
            for (size_t i = 0; i < length(); i++) {
                out.push_back(String(c_str[i]));
            }

            return out;
        }

        size_t last_end = 0;
        Optional<size_t> maybe_location;
        while ((maybe_location = find(delimiter, last_end))) {
            out.push_back(slice(last_end, *maybe_location));
            last_end = *maybe_location + delimiter.length();
        }

        out.push_back(slice(last_end, length()));

        return out;
    }

    String replace(const String& target, const String& replacement) const {
        String out;
        size_t last_end = 0;
        Optional<size_t> maybe_location;

        while ((maybe_location = find(target, last_end))) {
            out.append(slice(last_end, *maybe_location));
            out.append(replacement);
            last_end = *maybe_location + target.length();
        }

        out.append(slice(last_end, length()));

        return out;
    }

    Optional<size_t> find(const String& string, size_t start = 0) const {
        size_t sample_size = string.length();
        if (sample_size > length()) return Optional<size_t>();

        for (size_t i = start; i <= length() - sample_size; i++) {
            if (slice(i, i + sample_size) == string) return Optional<size_t>(i);
        }

        return Optional<size_t>();
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
        return slice(0, n);
    }

    String last_n(size_t n) {
        size_t len = length();
        if (!len) return "";

        size_t start = (n > len) ? 0 : len - n;
        return slice(start, len);
    }

    String slice(size_t start, size_t end) const {
        if (start == end) return String("");

        ASSERT(start < end, "Be orderly!");
        char* str = (char*)calloc(end - start + 1, 1);

        memcpy(str,  c_str + start, end - start);
        str[end - start] = '\0';

        return String::move_from(str);
    }

    bool starts_with(String target) const {
        if (length() < target.length()) return false;
        return slice(0, target.length()) == target;
    }

    static String from_fd(int fd, ssize_t limit = -1, size_t chunk_size = 1024) {
        String string;

        if (!limit) return string;

        char* buf = nullptr;
        size_t total_read = 0;

        while (true) {
            size_t bytes_to_read = chunk_size;

            if (limit > 0) {
                // We have a real limit....
                if (total_read >= (size_t)limit) break;
                bytes_to_read = min(bytes_to_read, limit - total_read);

            }

            buf = (char*) malloc(chunk_size + 1);

            ssize_t bytes_read = read(fd, buf, chunk_size);
            ASSERT(bytes_read >= 0, "ERROR READING!");
            if (bytes_read == 0) break;

            buf[bytes_read] = '\0';
            string.append(buf);
            total_read += bytes_read;

            free(buf);
            if ((size_t)bytes_read < chunk_size) break;
        }

        return string;
    }

    const char* as_c() const {
        return c_str;
    }
    
    float to_float() const {
        // TODO: Error check or DIE!!!!
        char* end;
        return strtof(c_str, &end);
    }

    int to_int() const {
        char* end;
        return strtod(c_str, &end);
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