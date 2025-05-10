#include <string>
#include "cpp.h"

#define PUSH_LONER(char, token) case char: push_loner(token); break;

void CPPParser::parse() {
    tokens.clear();
    char_idx = 0;

    char c = '\0';

    while ((c = eat_char())) {
        switch (c) {
            case ' ':
                reuse_or_push_if_differs(TokenType::WHITESPACE);
                break;
            
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
                if (
                    active_token.type != TokenType::NUMBER
                    && active_token.type != TokenType::SYMBOL
                ) {
                    // Numbers can continue symbols but not establish them
                    push_loner(TokenType::NUMBER);
                }
                break;

            PUSH_LONER('#', TokenType::POUND);
            PUSH_LONER('"', TokenType::DOUBLE_QUOTE);
            PUSH_LONER('\'', TokenType::SINGLE_QUOTE);
            PUSH_LONER(';', TokenType::SEMICOLON);
            PUSH_LONER(':', TokenType::COLON);
            PUSH_LONER('(', TokenType::OPEN_PAREN);
            PUSH_LONER(')', TokenType::CLOSE_PAREN);
            PUSH_LONER('[', TokenType::OPEN_BRACKET);
            PUSH_LONER(']', TokenType::CLOSE_BRACKET);
            PUSH_LONER('{', TokenType::OPEN_BRACE);
            PUSH_LONER('}', TokenType::CLOSE_BRACE);
            PUSH_LONER('=', TokenType::EQUALS);
            PUSH_LONER('\n', TokenType::NEWLINE);
            PUSH_LONER('*', TokenType::ASTERISK);
            PUSH_LONER('.', TokenType::DOT);
            PUSH_LONER('/', TokenType::SLASH);

            default:
                reuse_or_push_if_differs(TokenType::SYMBOL);
                break;
        }

        active_token.text.append(c);


        // printf("Parsing lol %c\n", c);
    }

    push_token();

    size_t token_idx = 0;
    bool commenting = false;

    while (token_idx < tokens.size()) {
        Token* token = &tokens[token_idx++];

        // printf("'%s'\n", token.text.as_c());

        switch (token->type) {
            case TokenType::SLASH:
                if (tokens[token_idx].type == TokenType::SLASH) {
                    commenting = true;
                }

                break;

            case TokenType::NEWLINE:
                commenting = false;
                break;
        }

        token->commented = commenting;
    }
}