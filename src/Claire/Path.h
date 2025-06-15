#pragma once
#include "Claire/String.h"

class Path {
public:
    static String join(String a, String b) {
        if (a == "./") return b;
        if (!a.length()) return b;
        return a + "/" + b;
    }

    static String global(String path) {
        if (path.starts_with("/")) return path;
        path = path.replace("./", "");
        path = join(cwd(), path);

        return path;
    }

    static String cwd() {
        char* buf = (char*)calloc(PATH_MAX, 1);
        getcwd(buf, PATH_MAX);
        return String::move_from(buf);
    }

    static String dir_path(String path) {
        size_t last_slash_dest = 0;

        while (true) {
            auto maybe_last = path.find("/", last_slash_dest + 1);
            if (!maybe_last) break;
            last_slash_dest = *maybe_last;
        }

        ASSERT(last_slash_dest, "Wheres the slashes");

        return path.slice(0, last_slash_dest);
    }

    static String exec_path() {
        char* exec_path = realpath("/proc/self/exe", nullptr);
        return String::move_from(exec_path);
    }

    static String exec_relative(String path) {
        return join(dir_path(exec_path()), path);
    }
};
