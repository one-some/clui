#pragma once
#include <sys/mman.h>
#include <sys/stat.h>
#include <cstdio>
#include <unistd.h>
#include <vector>

#include "Claire/Assert.h"
#include <string.h>
#include "UI/Container/Container.h"
#include "fontglobal.h"


class LogContainer : public Container {
    public:
        RayLib::Font font = Font::the();
        int32_t font_size_px = 16;

        virtual void draw_self();

        static void swallow_stdout();
        static void flush_stdout();

    private:
        static int outfd;
        static std::vector<const char*> output_lines;
};
