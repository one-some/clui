#pragma once

#include <vector>
#include <cstddef>
#include "string.h"

enum TokenType {
    WHITESPACE,
    SYMBOL,
    SEMICOLON,
};

class Node {
    public:
        String text;
        TokenType token_type = TokenType::SYMBOL;
};

class CPPParser {
    public:
        String input;
        size_t i = 0;
        std::vector<Node> nodes;
        
        CPPParser(String input) {
            this->input = input;
        }

        inline char eat() {
            return input.as_c()[i++];
        }

        inline char peek() {
            return input.as_c()[i];
        }


        void parse();
};