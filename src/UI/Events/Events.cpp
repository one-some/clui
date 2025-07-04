// TODO: Do we reaaaaally need this

#include "UI/Events/Events.h"
#include "vector2.h"

const int32_t MouseHoverEvent::hover_time_frames = 30;
int32_t MouseHoverEvent::hover_start_frame = 0;
Vector2 MouseHoverEvent::old_position = Vector2::zero();
bool MouseHoverEvent::dead_mouse = false;


void MouseHoverEvent::maybe_propagate(
    Container* cont,
    Vector2 mouse_pos,
    int32_t frame_no
) {
    // Should an event be responsible for propagating itself...?

    bool mouse_has_moved = mouse_pos != old_position;
    old_position = mouse_pos;


    if (dead_mouse) {
        if (!mouse_has_moved) return;
        dead_mouse = false;

        // Let's unhover!
        auto event = MouseHoverEvent(false);
        cont->dispatch_event(event);
        return;
    }

    if (mouse_has_moved) {
        hover_start_frame = frame_no;
        return;
    }

    if (frame_no - hover_start_frame >= hover_time_frames) {
        dead_mouse = true;
        // Let's hover!

        auto event = MouseHoverEvent(true);
        cont->dispatch_event(event);
        return;
    }
}