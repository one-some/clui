#pragma once
#include "Claire/String.h"

class Path {
public:
    static String join(String a, String b) {
        if (a == "./") return b;
        if (!a.length()) return b;
        a.append("/");
        a.append(b.as_c());
        return a;
    }
};