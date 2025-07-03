// TODO: Do we reaaaaally need this

#include "UI/Events/Events.h"

const int32_t MouseHoverEvent::hover_time_frames = 120;
int32_t MouseHoverEvent::hover_start_frame = 0;
Vector2 MouseHoverEvent::old_position = Vector2::zero();
bool MouseHoverEvent::done = false;