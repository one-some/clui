#include "Claire/JSON/JSON.h"

JSONValue* JSONObject::get(String key) const {
    auto it = data->find(key);
    if (it == data->end()) {
        printf("Keys: ");
        for (auto& pair : *data) {
            printf("'%s' ", pair.first.as_c());
        }
        printf("\n");
        ASSERT_NOT_REACHED("Bad key '%s' for object", key.as_c());
    }

    return it->second.get();
}

void JSONObject::set(const String& key, std::unique_ptr<JSONValue> val) {
    //fprintf(stderr, "--- Breakpoint: JSONObject::set for key ---\n");
    //key.print_state("MAP KEY BEING USED");
    //fprintf(stderr, "------------------------------------------\n");

    (*data)[key] = std::move(val);

}

void JSONObject::set(const String& key, String val) {
    set(key, std::make_unique<JSONString>(val));
}

void JSONObject::set(const String& key, double val) {
    set(key, std::make_unique<JSONNumber>(val));
}

String JSONObject::to_string() {
    String out = "{";

    auto iterator = data->begin();
    while (iterator != data->end()) {
        out.append(JSONString::to_string(iterator->first));
        out.append(": ");
        out.append(iterator->second->to_string());

        iterator++;

        if (iterator != data->end()) {
            out.append(", ");
        }
    }

    out.append("}");
    return out;
}

String JSONArray::to_string() {
    String out = "[";

    auto iterator = data->begin();
    while (iterator != data->end()) {
        out.append((*iterator)->to_string());

        iterator++;

        if (iterator != data->end()) {
            out.append(", ");
        }
    }

    out.append("]");
    return out;
}
