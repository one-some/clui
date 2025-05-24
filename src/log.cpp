#include "log.h"
#include "color.h"

#define DONT_EAT_LOG

int LogContainer::outfd = -1;
std::vector<const char*> LogContainer::output_lines;

void LogContainer::swallow_stdout() {
#ifndef DONT_EAT_LOG
    // It begins....
    outfd = memfd_create("clog", 0);
    fflush(stdout);
    dup2(outfd, STDOUT_FILENO);
#endif
}

void LogContainer::flush_stdout() {
#ifndef DONT_EAT_LOG
    fflush(stdout);

    struct stat sb;
    ASSERT(fstat(outfd, &sb) != -1, "BAD FSTAT");
    if (!sb.st_size) return;

    char* ptr = (char*)mmap(NULL, sb.st_size, PROT_READ, MAP_SHARED, outfd, 0);

    char* out = (char*)malloc(sb.st_size + 1);
    memcpy(out, ptr, sb.st_size);
    out[sb.st_size] = '\0';

    output_lines.push_back(out);
    // MAJOR HACK: Bypass the swallowing of stdout by redirecting to stderr
    fprintf(stderr, "%s", out);

    munmap(ptr, sb.st_size);

    ASSERT(ftruncate(outfd, 0) != -1, "BAD FTRUNCATE");
    ASSERT(lseek(outfd, 0, SEEK_SET) != -1, "BAD LSEEK");
#endif
}

void LogContainer::draw_self() {
    Vector2 pos = position->get_global();

    for (size_t i=0; i<output_lines.size();i++) {
        RayLib::DrawTextEx(
            font,
            output_lines[i],
            { (float)pos.x + 4, (float)pos.y + (font_size_px * i) },
            (float)font_size_px,
            0,
            Colors::FG.to_ray()
        );
    }
}