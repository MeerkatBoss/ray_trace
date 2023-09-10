#include "ray_trace/color.h"

const Color Color::Black   = Color::fromNormalized(0, 0, 0);
const Color Color::Red     = Color::fromNormalized(1, 0, 0);
const Color Color::Green   = Color::fromNormalized(0, 1, 0);
const Color Color::Blue    = Color::fromNormalized(0, 0, 1);
const Color Color::Yellow  = Color::fromNormalized(1, 1, 0);
const Color Color::Cyan    = Color::fromNormalized(0, 1, 1);
const Color Color::Magenta = Color::fromNormalized(1, 0, 1);
const Color Color::White   = Color::fromNormalized(1, 1, 1);

