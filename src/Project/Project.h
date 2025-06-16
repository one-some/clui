#pragma once
#include "Claire/String.h"
#include "Claire/Path.h"
#include "Claire/File.h"
#include "Claire/JSON/JSON.h"
#include "Actions/EditorActions.h"
#include "Dep/raylib.h"

std::map<String, int> keys_by_name = {
    {"APOSTROPHE", RayLib::KEY_APOSTROPHE},
    {"COMMA", RayLib::KEY_COMMA},
    {"MINUS", RayLib::KEY_MINUS},
    {"PERIOD", RayLib::KEY_PERIOD},
    {"SLASH", RayLib::KEY_SLASH},
    {"ZERO", RayLib::KEY_ZERO},
    {"ONE", RayLib::KEY_ONE},
    {"TWO", RayLib::KEY_TWO},
    {"THREE", RayLib::KEY_THREE},
    {"FOUR", RayLib::KEY_FOUR},
    {"FIVE", RayLib::KEY_FIVE},
    {"SIX", RayLib::KEY_SIX},
    {"SEVEN", RayLib::KEY_SEVEN},
    {"EIGHT", RayLib::KEY_EIGHT},
    {"NINE", RayLib::KEY_NINE},
    {"SEMICOLON", RayLib::KEY_SEMICOLON},
    {"EQUAL", RayLib::KEY_EQUAL},
    {"A", RayLib::KEY_A},
    {"B", RayLib::KEY_B},
    {"C", RayLib::KEY_C},
    {"D", RayLib::KEY_D},
    {"E", RayLib::KEY_E},
    {"F", RayLib::KEY_F},
    {"G", RayLib::KEY_G},
    {"H", RayLib::KEY_H},
    {"I", RayLib::KEY_I},
    {"J", RayLib::KEY_J},
    {"K", RayLib::KEY_K},
    {"L", RayLib::KEY_L},
    {"M", RayLib::KEY_M},
    {"N", RayLib::KEY_N},
    {"O", RayLib::KEY_O},
    {"P", RayLib::KEY_P},
    {"Q", RayLib::KEY_Q},
    {"R", RayLib::KEY_R},
    {"S", RayLib::KEY_S},
    {"T", RayLib::KEY_T},
    {"U", RayLib::KEY_U},
    {"V", RayLib::KEY_V},
    {"W", RayLib::KEY_W},
    {"X", RayLib::KEY_X},
    {"Y", RayLib::KEY_Y},
    {"Z", RayLib::KEY_Z},
    {"LEFT_BRACKET", RayLib::KEY_LEFT_BRACKET},
    {"BACKSLASH", RayLib::KEY_BACKSLASH},
    {"RIGHT_BRACKET", RayLib::KEY_RIGHT_BRACKET},
    {"GRAVE", RayLib::KEY_GRAVE},
    {"SPACE", RayLib::KEY_SPACE},
    {"ESCAPE", RayLib::KEY_ESCAPE},
    {"ENTER", RayLib::KEY_ENTER},
    {"TAB", RayLib::KEY_TAB},
    {"BACKSPACE", RayLib::KEY_BACKSPACE},
    {"INSERT", RayLib::KEY_INSERT},
    {"DELETE", RayLib::KEY_DELETE},
    {"RIGHT", RayLib::KEY_RIGHT},
    {"LEFT", RayLib::KEY_LEFT},
    {"DOWN", RayLib::KEY_DOWN},
    {"UP", RayLib::KEY_UP},
    {"PAGE_UP", RayLib::KEY_PAGE_UP},
    {"PAGE_DOWN", RayLib::KEY_PAGE_DOWN},
    {"HOME", RayLib::KEY_HOME},
    {"END", RayLib::KEY_END},
    {"CAPS_LOCK", RayLib::KEY_CAPS_LOCK},
    {"SCROLL_LOCK", RayLib::KEY_SCROLL_LOCK},
    {"NUM_LOCK", RayLib::KEY_NUM_LOCK},
    {"PRINT_SCREEN", RayLib::KEY_PRINT_SCREEN},
    {"PAUSE", RayLib::KEY_PAUSE},
    {"F1", RayLib::KEY_F1},
    {"F2", RayLib::KEY_F2},
    {"F3", RayLib::KEY_F3},
    {"F4", RayLib::KEY_F4},
    {"F5", RayLib::KEY_F5},
    {"F6", RayLib::KEY_F6},
    {"F7", RayLib::KEY_F7},
    {"F8", RayLib::KEY_F8},
    {"F9", RayLib::KEY_F9},
    {"F10", RayLib::KEY_F10},
    {"F11", RayLib::KEY_F11},
    {"F12", RayLib::KEY_F12},
    {"LEFT_SHIFT", RayLib::KEY_LEFT_SHIFT},
    {"LEFT_CONTROL", RayLib::KEY_LEFT_CONTROL},
    {"LEFT_ALT", RayLib::KEY_LEFT_ALT},
    {"LEFT_SUPER", RayLib::KEY_LEFT_SUPER},
    {"RIGHT_SHIFT", RayLib::KEY_RIGHT_SHIFT},
    {"RIGHT_CONTROL", RayLib::KEY_RIGHT_CONTROL},
    {"RIGHT_ALT", RayLib::KEY_RIGHT_ALT},
    {"RIGHT_SUPER", RayLib::KEY_RIGHT_SUPER},
    {"KB_MENU", RayLib::KEY_KB_MENU},
    {"KP_0", RayLib::KEY_KP_0},
    {"KP_1", RayLib::KEY_KP_1},
    {"KP_2", RayLib::KEY_KP_2},
    {"KP_3", RayLib::KEY_KP_3},
    {"KP_4", RayLib::KEY_KP_4},
    {"KP_5", RayLib::KEY_KP_5},
    {"KP_6", RayLib::KEY_KP_6},
    {"KP_7", RayLib::KEY_KP_7},
    {"KP_8", RayLib::KEY_KP_8},
    {"KP_9", RayLib::KEY_KP_9},
    {"KP_DECIMAL", RayLib::KEY_KP_DECIMAL},
    {"KP_DIVIDE", RayLib::KEY_KP_DIVIDE},
    {"KP_MULTIPLY", RayLib::KEY_KP_MULTIPLY},
    {"KP_SUBTRACT", RayLib::KEY_KP_SUBTRACT},
    {"KP_ADD", RayLib::KEY_KP_ADD},
    {"KP_ENTER", RayLib::KEY_KP_ENTER},
    {"KP_EQUAL", RayLib::KEY_KP_EQUAL},
};

class Project {
public:
    Project(String path) : project_path(path) {
        auto claire_dir = Path::join(path, ".claire");

        printf("Opening project...\n");

        if (!Path::is_dir(claire_dir)) return;
        printf("Rich claire! :D\n");

        auto keybind_path = Path::join(claire_dir, "hotkeys.json");
        if (Path::is_file(keybind_path)) load_keybinds(keybind_path);
    }

    void do_keybind_action(int key) {
        if (keybound_actions.count(key) <= 0) return;
        keybound_actions[key]->execute();
    }

private:
    std::map<int, std::unique_ptr<Action>> keybound_actions;

    void load_keybinds(String path) {
        // I HATE THIS. FIX THIS SOON.
        auto _object = JSONParser(File(path).read()).parse();
        auto object = _object->as<JSONObject>();

        for (auto& pair : *(object->data)) {
            int raylib_key = keys_by_name[pair.first];
            keybound_actions[raylib_key] = action_from_json(pair.second->as<JSONObject>());
        }
    }

    std::unique_ptr<Action> action_from_json(JSONObject* json) {
        auto context_hash = json->get<JSONString>("context")->value.hash();

        switch (context_hash) {
            case String::hash("shell"):
                return std::make_unique<CommandAction>(json->get<JSONString>("command")->value);
        }

        ASSERT_NOT_REACHED("Unknown action context");
    }

    String project_path;
};
