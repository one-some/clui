#pragma once
#include "Claire/String.h"
#include "Claire/Path.h"
#include "Claire/File.h"
#include "Claire/JSON/JSON.h"
#include "Dep/raylib.h"

class Project {
public:
    // std::map<
    Project(String path) : project_path(path) {
        auto claire_dir = Path::join(path, ".claire");

        printf("Opening project...\n");

        if (!Path::is_dir(claire_dir)) return;
        printf("Rich claire! :D\n");

        auto keybind_path = Path::join(claire_dir, "hotkeys.json");
        if (Path::is_file(keybind_path)) load_keybinds(keybind_path);
    }

private:
    void load_keybinds(String path) {
        // I HATE THIS. FIX THIS SOON.
        auto _object = JSONParser(File(path).read()).parse();
        auto object = _object->as<JSONObject>();
        RayLib::

        for (auto& pair : *(object->data)) {
            printf("%s: %s\n", pair.first.as_c(), pair.second->to_string().as_c());
        }
    }

    String project_path;
};