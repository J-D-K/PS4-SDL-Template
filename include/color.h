#pragma once

// I prefer to work with hex colors so these are for that
#define GET_RED(x) x >> 24 & 0xFF
#define GET_GREEN(x) x >> 16 & 0xFF
#define GET_BLUE(x) x >> 8 & 0xFF
#define GET_ALPHA(x) x & 0xFF
#define CREATE_COLOR(r, g, b, a) r << 24 | g << 16 | b << 8 | a
