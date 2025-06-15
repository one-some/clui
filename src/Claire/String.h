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

    void print_state(const char* event) const {
	return;
        fprintf(stderr, "[Test STRING EVENT] %-20s | Addr: %p | Cap: %-4zu | Len: %-4zu | Data: \"%s\"\n",
            event, (void*)this, capacity, length(), c_str ? c_str : "null");
    }

    String() {
        capacity = 16;
        c_str = (char*)calloc(capacity, 1);

        print_state("Default const");
    }

    String(char c) {
        capacity = 4;
        c_str = (char*)calloc(capacity, 1);
        c_str[0] = c;
        c_str[1] = '\0';

        print_state("Char const");
    }

    String(const char* in_c_str) {
        if (!in_c_str) {
            capacity = 16;
            c_str = (char*) calloc(capacity, 1);
            print_state("const char* null");
            return;
        }

        capacity = strlen(in_c_str) + 1;
        c_str = (char*)calloc(capacity, 1);
        strcpy(c_str, in_c_str);

        print_state("const char* const");
    }

    String(const String& that) {
        ASSERT(that.c_str, "No CSTR");

        capacity = that.capacity;
        c_str = (char*)calloc(capacity, 1);
        ASSERT(c_str, "Can't allocate");

        strcpy(c_str, that.c_str);
        print_state("Copy const deep");
    }

    String(String&& that) noexcept : String() {
        ASSERT(that.c_str, "Source has null cstr");

        std::swap(capacity, that.capacity);
        std::swap(c_str, that.c_str);

        print_state("Move const");
    }

    String& operator=(String that) noexcept {
        ASSERT(c_str, "No dest cstr");
        ASSERT(that.c_str, "No dest cstr");
        std::swap(c_str, that.c_str);
        std::swap(capacity, that.capacity);
        print_state("Assignment");
        return *this;
    }

    ~String() {
        ASSERT(c_str, "Destroying corrupt string");
        print_state("GONE");
        free(c_str);
    }

    void operator+=(const String& that) {
        append(that);
    }

    static String from_double(double val, int round_places = -1) {
        int len = snprintf(NULL, 0, "%f", val);
        char* buf = (char*) calloc(len + 1, 1);
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
        char* buf = (char*) calloc(len + 1, 1);
        snprintf(buf, len + 1, "%d", val);
        return String::move_from(buf);
    }

    static String from_number_klutz(double val) {
        if (val == std::floor(val)) return from_int((int) val);
        return from_double(val);
    }

    static String move_from(char* in_c_str) {
        if (!in_c_str) return String("");

        return String(in_c_str, strlen(in_c_str) + 1);

        // TODO: Make "empty" constructor
        // String out;
        // free(out.c_str);

        // out.capacity = strlen(in_c_str) + 1;
        // out.c_str = in_c_str;
        // return out;
    }
    
    char operator[](size_t index) {
        return c_str[index];
    }

    bool operator==(const String& that) const {
        return *this == that.c_str;
    }

    bool operator==(const char* that) const {
        ASSERT(c_str && that, "BAD PTRS");
        return strcmp(c_str, that) == 0;
    }

    friend bool operator<(const String& lhs, const String& rhs) {
        return strcmp(lhs.as_c(), rhs.as_c()) < 0;
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
        return c_str ? strlen(c_str) : 0;
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

                char* new_string = (char*)calloc(new_len + 1, 1);
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

    String truncated(size_t to=100) {
        if (length() <= to) return String(*this);

        size_t left = length() - to;
        String out = first_n(to);
        out.append(" ... (plus ");
        out.append(String::from_int((int)left));
        out.append(" characters)");
        
        return out;
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

            buf = (char*)calloc(chunk_size + 1, 1);

            ssize_t bytes_read = read(fd, buf, chunk_size);
            ASSERT(bytes_read >= 0, "ERROR READING!");
            if (bytes_read == 0) break;

            buf[bytes_read] = '\0';
            string.append(buf);
            total_read += (size_t)bytes_read;

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
        return (int)strtod(c_str, &end);
    }

private:
    char* c_str = nullptr;
    size_t capacity = 0;

    String(char* stolen_ptr, size_t stolen_capacity) : c_str(stolen_ptr), capacity(stolen_capacity) {
        print_state("Private move_from const");
    }

    void expand_for(size_t extra_chars) {
        size_t old_len = strlen(c_str);
        if (capacity >= (old_len + 1 + extra_chars)) return;

        capacity = (capacity + extra_chars) * 2;

        char* new_string = (char*)calloc(capacity, 1);
        memcpy(new_string, c_str, old_len + 1);

        free(c_str);
        c_str = new_string;
    }
};

inline String operator+(const String& lhs, const String& rhs) {
    String out = String(lhs);
    out.append(rhs);
    return out;
}
