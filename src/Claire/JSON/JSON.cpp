#include "Claire/JSON/JSON.h"

JSONValue* JSONObject::get(String key) {
    bool valid = data->find(key) != data->end();
    if (!valid) {
        printf("Keys: ");
        for (auto& pair : *data) {
            printf("'%s' ", pair.first.as_c());
        }
        printf("\n");
        ASSERT(valid, "Bad key '%s' for object", key.as_c());
    }

    return (*data)[key];
}

void JSONObject::set(String key, JSONValue* val) {
    (*data)[key] = val;
}

void JSONObject::set(String key, String val) {
    set(key, new JSONString(val));
}

void JSONObject::set(String key, double val) {
    set(key, new JSONNumber(val));
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