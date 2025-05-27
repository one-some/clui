#define _XOPEN_SOURCE 700
#pragma once

#include <cstddef>
#include <cstdio>
#include <sys/types.h>
#include <dirent.h>
#include <vector>
#include "Claire/Assert.h"
#include "Claire/String.h"

enum DirectoryChildType {
    TYPE_FILE,
    TYPE_DIRECTORY,
};

struct DirectoryChild {
    DirectoryChildType type;
    String name;
};

class Directory {
    public:
        String path;

        Directory(String path) {
            this->path = path;
        }

        std::vector<DirectoryChild> list() {
            DIR* dp = opendir(path.as_c());
            ASSERT(dp, "Couldn't open directory");

            struct dirent* entry;
            std::vector<DirectoryChild> out;

            while ((entry = readdir(dp)) != NULL) {
                if (entry->d_name[0] == '.') continue;
                //entry->d_type = DT_DIR
                out.push_back({
                    entry->d_type == DT_DIR ? DirectoryChildType::TYPE_DIRECTORY : DirectoryChildType::TYPE_FILE,
                    String(entry->d_name),
                });
            }

            closedir(dp);

            return out;
        }
};