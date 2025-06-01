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
        T* ptr = dynamic_cast<T*>(this);
        if (!ptr) printf("JTYPE: %s\n", typeid(T).name());
        ASSERT(ptr, "Unable to cast JSON value in as...");
        return ptr;
    }
};

class JSONObject : public JSONValue {
public:
    std::map<String, JSONValue*>* data = new std::map<String, JSONValue*>;

    JSONValue* get(String key);
    void set(String key, JSONValue* val);

    // What a cute shortcut!
    template<JSONValueDerivative T>
    T* get(String key) {
        return get(key)->as<T>();
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
        printf("Parsing value:");
        char first_char = text[i];
        
        switch (first_char) {
            case '{':
                printf("Object!\n");
                return parse_object();
            case '"':
            case '\'':
                printf("String!\n");
                return parse_string();
        }

        if (String::is_number(first_char)) {
            printf("Number!\n");
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
            object->set(key->value, value);

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