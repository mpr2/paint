#include <cmath>
#include "paint.h"

void put_pixel(unsigned char *buffer, int x, int y, float color[4]) {
    if (x < 0 || y < 0 || x >= WIDTH || y >= HEIGHT) {
        return;
    }
    // buffer indices grow from bottom to top
    // while screen coordinates grow from top to bottom
    int pos = 4 * ((HEIGHT - 1 - y) * WIDTH + x);

    // convert color components from [0..1] to [0..255]
    buffer[pos]   = (unsigned char) (color[0] * 255.0);
    buffer[pos+1] = (unsigned char) (color[1] * 255.0);
    buffer[pos+2] = (unsigned char) (color[2] * 255.0);
    buffer[pos+3] = (unsigned char) (color[3] * 255.0);
}

void bresenham_line(unsigned char *buffer, int p0x, int p0y, int p1x, int p1y, float color[4]) {
    if (abs(p1x - p0x) > abs(p1y - p0y)) {
        int x0 = p0x;
        int x1 = p1x;
        int y0 = p0y;
        int y1 = p1y;
        if (p0x > p1x) {
            x0 = p1x;
            x1 = p0x;
            y0 = p1y;
            y1 = p0y;
        }
        int dx = x1 - x0;
        int dy = y1 - y0;
        int dir = 1;
        if (dy < 0) {
            dy = -dy;
            dir = -1;
        }

        if (dx != 0) {
            int y = y0;
            int p = 2*dy - dx;
            for (int i = 0; i < dx; i++) {
                put_pixel(buffer, x0 + i, y, color);
                if (p >= 0) {
                    y += dir;
                    p = p - 2*dx;
                }
                p = p + 2*dy;
            }
        }
    }
    else {
        int x0 = p0x;
        int x1 = p1x;
        int y0 = p0y;
        int y1 = p1y;
        if (p0y > p1y) {
            x0 = p1x;
            x1 = p0x;
            y0 = p1y;
            y1 = p0y;
        }
        int dx = x1 - x0;
        int dy = y1 - y0;
        int dir = 1;
        if (dx < 0) {
            dx = -dx;
            dir = -1;
        }

        if (dy != 0) {
            int x = x0;
            int p = 2*dx - dy;
            for (int i = 0; i < dy; i++) {
                put_pixel(buffer, x, y0 + i, color);
                if (p >= 0) {
                    x += dir;
                    p = p - 2*dy;
                }
                p = p + 2*dx;
            }
        }
    }
}

void midpoint_circle(unsigned char *buffer, float x0, float y0, float radius, float color[4]) {
    float x = 0;
    float y = -radius;
    float p = -radius;
    while (x < -y) {
        if (p > 0) {
            y += 1;
            p += 2*(x+y) + 1;
        }
        else {
            p += 2*x + 1;
        }

        put_pixel(buffer, (int)(x0 + x), (int)(y0 + y), color);
        put_pixel(buffer, (int)(x0 - x), (int)(y0 + y), color);
        put_pixel(buffer, (int)(x0 + x), (int)(y0 - y), color);
        put_pixel(buffer, (int)(x0 - x), (int)(y0 - y), color);
        put_pixel(buffer, (int)(x0 + y), (int)(y0 + x), color);
        put_pixel(buffer, (int)(x0 - y), (int)(y0 + x), color);
        put_pixel(buffer, (int)(x0 + y), (int)(y0 - x), color);
        put_pixel(buffer, (int)(x0 - y), (int)(y0 - x), color);
        x += 1;
    }
}
