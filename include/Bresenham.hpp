#pragma once
#include "RasterBuffer.hpp"
#include <cstdint>
#include <cstdlib>

namespace Bresenham {

inline void line(RasterBuffer<uint8_t>& rb, int x0, int y0, int x1, int y1, uint8_t color=255) {
    int dx = std::abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -std::abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;
    for (;;) {
        rb.set_pixel(x0,y0,color);
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2*err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

// Midpoint/Bresenham circle
inline void circle(RasterBuffer<uint8_t>& rb, int cx, int cy, int radius, uint8_t color=255) {
    int x = radius;
    int y = 0;
    int err = 1 - x;

    auto plot8 = [&](int px, int py) {
        rb.set_pixel(cx+px, cy+py, color);
        rb.set_pixel(cx-px, cy+py, color);
        rb.set_pixel(cx+px, cy-py, color);
        rb.set_pixel(cx-px, cy-py, color);
        rb.set_pixel(cx+py, cy+px, color);
        rb.set_pixel(cx-py, cy+px, color);
        rb.set_pixel(cx+py, cy-px, color);
        rb.set_pixel(cx-py, cy-px, color);
    };

    while (x >= y) {
        plot8(x,y);
        ++y;
        if (err < 0) {
            err += 2*y + 1;
        } else {
            --x;
            err += 2*(y - x + 1);
        }
    }
}

} // namespace
