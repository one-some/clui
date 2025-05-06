#define _XOPEN_SOURCE 700
#pragma once

#include <cstddef>
#include <cstdio>
#include <sys/types.h>
#include <dirent.h>
#include <vector>
#include "Claire/Assert.h"
#include "Claire/String.h"

class Directory {
    public:
        const char* path = "";

        Directory(const char* path) {
            this->path = path;
        }

        std::vector<String> list() {
            DIR* dp = opendir(path);
            ASSERT(dp, "Couldn't open directory");

            struct dirent* entry;
            std::vector<String> out;

            while ((entry = readdir(dp)) != NULL) {
                out.push_back(String(entry->d_name));
            }

            closedir(dp);

            return out;
        }
};