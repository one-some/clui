#include "Claire/JSON/JSON.h"

JSONValue* JSONObject::get(const char* key) {
    return (*data)[key];
}

void JSONObject::set(const char* key, JSONValue* val) {
    (*data)[key] = val;
}