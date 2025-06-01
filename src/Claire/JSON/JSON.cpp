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