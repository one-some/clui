// TODO: Migrate to smart ptrs...

#pragma once

#include <map>
#include <memory>
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
    // Sharedptr for values??
    std::unique_ptr<std::map<String, std::unique_ptr<JSONValue>>> data = std::make_unique<std::map<String, std::unique_ptr<JSONValue>>>();

    JSONValue* get(String key);
    void set(const String& key, std::unique_ptr<JSONValue> val);
    void set(const String& key, String val);
    void set(const String& key, double val);

    template<JSONValueDerivative T, typename... Args>
    T* set_new(const String& key, Args&&... args) {
        auto ptr = std::make_unique<T>(std::forward<Args>(args)...);
        T* raw_ptr = ptr.get();

        set(key, std::move(ptr));
        return raw_ptr;
    }

    // What a cute shortcut!
    template<JSONValueDerivative T>
    T* get(String key) {
        return get(key)->as<T>();
    }

    String to_string() override;
};

class JSONArray : public JSONValue {
public:
    std::unique_ptr<std::vector<std::unique_ptr<JSONValue>>> data = std::make_unique<std::vector<std::unique_ptr<JSONValue>>>();

    void append(std::unique_ptr<JSONValue> val) {
        data->push_back(std::move(val));
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
        s = s.replace("\n", "\\n");
        // TODO: ????
        s = s.replace("\\?", "\\\\?");
        s = s.replace("\"", "\\\"");

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
};

class JSONBoolean : public JSONValue {
public:
    bool value;

    JSONBoolean(bool val): value(val) { }

    String to_string() override {
        return value ? "true" : "false";
    }
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

    std::unique_ptr<JSONValue> parse() {
        i = 0;
        return parse_value();
    }

    std::unique_ptr<JSONValue> parse_value() {
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

        if (String::is_letter(first_char)) {
            return parse_symbol();
        }
        
        printf("'%s'\n", text.as_c() + i);
        ASSERT_NOT_REACHED("No idea how to parse this value");
    }
    
    std::unique_ptr<JSONNumber> parse_number() {
        ASSERT(String::is_number(text[i]), "Number not a number! WE ARE NOT JS....");
        
        String num;
        while (String::is_number(text[i])) {
            num.add_char(text[i++]);
        }
        
        return std::make_unique<JSONNumber>(num.to_float());
    }

    std::unique_ptr<JSONValue> parse_symbol() {
        ASSERT(String::is_letter(text[i]), "This isn't how my limited worldview works");

        String symbol;
        while (String::is_letter(text[i])) {
            symbol.add_char(text[i++]);
        }

        if (symbol == "true") return std::make_unique<JSONBoolean>(true);
        if (symbol == "false") return std::make_unique<JSONBoolean>(false);
        if (symbol == "null") return std::make_unique<JSONNull>();

        ASSERT_NOT_REACHED("Unknown constant %s", symbol.as_c());
    }
    
    std::unique_ptr<JSONString> parse_string() {
        char starting_quote = text[i++];
        ASSERT(starting_quote == '"' || starting_quote == '\'', "Doesn't look like a string to me!");

        String out_string;
        while (true) {
            ASSERT(text[i], "It would really suck if the string ended here right");

            if (text[i] == starting_quote) {
                // Closing quote
                i++;
                break;
            } else if (text[i] != '\\') {
                // Normal chars
                out_string.add_char(text[i++]);
                continue;
            }

            // its a backslash. Brace for impact everybody
            i++;
            ASSERT(text[i], "Ok that's just weirdo behavior ending things right there.");

            switch (text[i]) {
                case '"':
                case '\\':
                case '/':
                    out_string.add_char(text[i]);
                    break;
                case 'n': out_string.add_char('\n'); break;
                case 'r': out_string.add_char('\r'); break;
                case 'b': out_string.add_char('\b'); break;
                case 'f': out_string.add_char('\f'); break;
                case 't': out_string.add_char('\t'); break;
                default:
                    ASSERT_NOT_REACHED("Lol \\%c what is that", text[i]);
            }

            i++;
        }
        
        return std::make_unique<JSONString>(out_string);
    }

    std::unique_ptr<JSONArray> parse_array() {
        ASSERT(text[i] == '[', "Doesn't look like an array to me!");

        auto array = std::make_unique<JSONArray>();

        zap_whitespace();
        if (text[++i] == ']') {
            i++;
            return array;
        }

        while (true) {
            zap_whitespace();
            auto val = parse_value();
            array->append(std::move(val));
            zap_whitespace();

            if (text[i] == ']') {
                i++;
                break;
            }

            ASSERT(text[i++] == ',', "Expected ',' or ']' after array value");
        }

        return array;
    }
    
    std::unique_ptr<JSONObject> parse_object() {
        ASSERT(text[i] == '{', "Doesn't look like an object to me!");

        auto object = std::make_unique<JSONObject>();
        if (text[++i] == '}') {
            i++;
            return object;
        }

        while (true) {
            zap_whitespace();
            auto key = parse_string();
            ASSERT(text[i++] == ':', "Expected ':' in object creation");

            zap_whitespace();
            auto value = parse_value();
            
            object->set(key->value, std::move(value));

            zap_whitespace();

            if (text[i] == '}') {
                i++;
                break;
            }
            
            ASSERT(text[i] == ',', "Expected ',' in object continuation");
            i++;
        }
        
        return object;
    }
};