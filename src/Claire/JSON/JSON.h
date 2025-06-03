// TODO: Migrate to smart ptrs...

#pragma once

#include <map>
#include <concepts>
#include "Claire/Assert.h"
#include "Claire/String.h"

class JSONValue;

template <typename T>
concept JSONValueDerivative = std::derived_from<T, JSONValue>;

class JSONValue {
public:
    virtual ~JSONValue() = default;

    virtual String to_string() = 0;
    
    template<JSONValueDerivative T>
    T* as() {
        T* ptr = dynamic_cast<T*>(this);
        ASSERT(ptr, "(.as<x>) Unable to cast JSON value to %s", typeid(T).name());
        return ptr;
    }
};

class JSONObject : public JSONValue {
public:
    std::map<String, JSONValue*>* data = new std::map<String, JSONValue*>;

    ~JSONObject() {
        delete data;
    } 

    JSONValue* get(String key);
    void set(String key, JSONValue* val);
    void set(String key, String val);
    void set(String key, double val);

    // What a cute shortcut!
    template<JSONValueDerivative T>
    T* get(String key) {
        return get(key)->as<T>();
    }

    String to_string() override;
};

class JSONArray : public JSONValue {
public:
    std::vector<JSONValue*>* data = new std::vector<JSONValue*>;

    void append(JSONValue* val) {
        data->push_back(val);
    }

    template<JSONValueDerivative T>
    T* get(int index) {
        return (*data)[index]->as<T>();
    }

    String to_string() override;
};

class JSONString : public JSONValue {
public:
    String value;
    
    JSONString(String val): value(val) { }

    static String to_string(String s) {
        String out = "\"";
        out.append(s);
        out.append("\"");
        return out;
    }

    String to_string() override {
        return JSONString::to_string(value);
    }
};

class JSONNumber : public JSONValue {
public:
    double value;
    
    JSONNumber(double val): value(val) { }

    String to_string() override {
        // HACK for JSON-RPC for LSP (id SHOULD NOT have a fractional part)
        return String::from_number_klutz(value);
    }
};

class JSONNull : public JSONValue {
public:
    String to_string() override {
        return "null";
    }

    static JSONNull* the() {
        // Awesome - Meyers' Singleton
        static JSONNull _the;
        return &_the;
    }

private:
    JSONNull() = default;
};

class JSONParser {
public:
    String text;
    size_t i = 0;

    JSONParser(String input) {
        text = input;
    }
    
    bool is_whitespace(char c) {
        if (c == ' ') return true;
        if (c == '\n') return true;
        if (c == '\r') return true;
        if (c == '\t') return true;
        return false;
    }
    
    void zap_whitespace() {
        while (is_whitespace(text[i])) {
            ASSERT(text[i], "My oh my thats the null terminator!");
            i++;
        }
        ASSERT(text[i], "My oh my thats the null terminator!");
    }

    JSONValue* parse() {
        i = 0;
        return parse_value();
    }

    JSONValue* parse_value() {
        char first_char = text[i];
        
        switch (first_char) {
            case '{':
                return parse_object();
            case '[':
                return parse_array();
            case '"':
            case '\'':
                return parse_string();
        }

        if (String::is_number(first_char)) {
            return parse_number();
        }
        
        printf("'%s'\n", text.as_c() + i);
        ASSERT_NOT_REACHED("No idea how to parse this value");
    }
    
    JSONNumber* parse_number() {
        ASSERT(String::is_number(text[i]), "Number not a number! WE ARE NOT JS....");
        
        String num;
        while (String::is_number(text[i])) {
            num.add_char(text[i]);
            i++;
        }
        
        return new JSONNumber(num.to_float());
    }
    
    JSONString* parse_string() {
        char starting_quote = text[i++];
        ASSERT(starting_quote == '"' || starting_quote == '\'', "Doesn't look like a string to me!");

        String out_string;
        while (text[i] != starting_quote) {
            ASSERT(text[i], "It would really suck if the string ended here right");
            out_string.add_char(text[i]);
            i++;
        }
        
        i++;
        
        return new JSONString(out_string);
    }

    JSONArray* parse_array() {
        ASSERT(text[i] == '[', "Doesn't look like an array to me!");

        JSONArray* array = new JSONArray();
        if (text[++i] == ']') return array;

        while (true) {
            zap_whitespace();
            JSONValue* val = parse_value();
            array->append(val);
            zap_whitespace();

            if (text[i] == ']') {
                break;
            }

            ASSERT(text[i++] == ',', "Expected ',' or ']' after array value");
        }

        i++;
        return array;
    }
    
    JSONObject* parse_object() {
        ASSERT(text[i] == '{', "Doesn't look like an object to me!");

        JSONObject* object = new JSONObject();
        if (text[++i] == '}') return object;

        while (true) {
            zap_whitespace();
            JSONString* key = parse_string();
            ASSERT(text[i++] == ':', "Expected ':' in object creation");

            zap_whitespace();
            JSONValue* value = parse_value();
            
            object->set(key->value, value);

            zap_whitespace();
            if (text[i] == '}') break;
            
            ASSERT(text[i] == ',', "Expected ',' in object continuation");
            i++;
        }
        
        return object;
    }
};