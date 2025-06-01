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
    
    template<JSONValueDerivative T>
    T* as() {
        return dynamic_cast<T*>(this);
    }
};

class JSONObject : public JSONValue {
public:
    std::map<const char*, JSONValue*>* data = new std::map<const char*, JSONValue*>;

    JSONValue* get(const char* key);
    void set(const char* key, JSONValue* val);

    // What a cute shortcut!
    template<JSONValueDerivative T>
    T* get(const char* key) {
        T* ptr = dynamic_cast<T*>(get(key));
        ASSERT(ptr, "Unable to cast JSON value...");
        return ptr;
    }
};

class JSONString : public JSONValue {
public:
    String value;
    
    JSONString(String val): value(val) { }
};

class JSONNumber : public JSONValue {
public:
    float value;
    
    JSONNumber(float val): value(val) { }
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
            case '"':
            case '\'':
                return parse_string();
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
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
        
        return new JSONNumber(num.as_float());
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
    
    JSONObject* parse_object() {
        ASSERT(text[i] == '{', "Doesn't look like an object to me!");

        JSONObject* object = new JSONObject();
        
        if (text[++i] == '}') return object;
        zap_whitespace();

        while (true) {
            printf("[1] XX%s\n", text.as_c() + i);
            zap_whitespace();
            JSONString* key = parse_string();
            printf("[2] XX%s\n", text.as_c() + i);
            ASSERT(text[i++] == ':', "Expected ':' in object creation");

            zap_whitespace();
            JSONValue* value = parse_value();
            
            printf("[3] XX%s\n", text.as_c() + i);
            object->set(key->value.as_c(), value);

            zap_whitespace();
            if (text[i] == '}') break;
            
            printf("'%c'\n", text[i]);
            printf("[4] XX%s\n", text.as_c() + i);
            
            ASSERT(text[i] == ',', "Expected ',' in object continuation");
            i++;
        }
        
        return object;
    }
};