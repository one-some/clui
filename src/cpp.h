#pragma once

#include <vector>
#include <cstddef>
#include "string.h"

enum TokenType {
    WHITESPACE,
    NEWLINE,
    SYMBOL,
    SEMICOLON,
    COLON,
    POUND,
    DOUBLE_QUOTE,
    SINGLE_QUOTE,
    NUMBER,
    OPEN_PAREN,
    CLOSE_PAREN,
    OPEN_BRACKET,
    CLOSE_BRACKET,
    OPEN_BRACE,
    CLOSE_BRACE,
    EQUALS,
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
        Node active_node;
        
        CPPParser(String input) {
            this->input = input;
        }

        void reuse_or_push_if_differs(TokenType token_type) {
            if (active_node.token_type == token_type) return;

            push_loner(token_type);
        }

        void push_loner(TokenType token_type) {
            push_token();
            active_node.token_type = token_type;
        }

        void push_token() {
            printf("Pushing with %i: '%s'\n", active_node.token_type, active_node.text.as_c());
            nodes.push_back(active_node);
            active_node = Node();
        }

        inline char eat() {
            return input.as_c()[i++];
        }

        inline char peek() {
            return input.as_c()[i];
        }


        void parse();
};