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
    ASTERISK,
    DOT,
    SLASH,
};

class Token {
    public:
        String text;
        TokenType type = TokenType::SYMBOL;
        bool commented = false;
};

class CPPParser {
    public:
        String input;
        std::vector<Token> tokens;
        Token active_token;
        size_t char_idx = 0;
        
        CPPParser(String input) {
            this->input = input;
        }

        void reuse_or_push_if_differs(TokenType token_type) {
            if (active_token.type == token_type) return;

            push_loner(token_type);
        }

        void push_loner(TokenType token_type) {
            push_token();
            active_token.type = token_type;
        }

        void push_token() {
            // printf("Pushing with %i: '%s'\n", active_token.token_type, active_token.text.as_c());
            tokens.push_back(active_token);
            active_token = Token();
        }

        inline char eat_char() {
            return input.as_c()[char_idx++];
        }

        inline char peek_char() {
            return input.as_c()[char_idx];
        }

        void parse();
};