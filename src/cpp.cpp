#include "cpp.h"

void CPPParser::parse() {
    char c = '\0';

    while ((c = eat())) {
        printf("Parsing lol %c\n", c);
    }
}