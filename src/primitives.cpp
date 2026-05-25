#include <cmath>
#include <queue>
#include "paint.h"
#include "primitives.h"

void put_pixel(unsigned char *buffer, int x, int y, Color color, int thickness) {
    for (int i = x - thickness; i <= x + thickness; i++) {
        for (int j = y - thickness; j <= y + thickness; j++) {
            if (i < 0 || j < 0 || i >= WIDTH || j >= HEIGHT) {
                continue;
            }
            int pos = 4 * (j * WIDTH + i);

            buffer[pos]   = color.r;
            buffer[pos+1] = color.g;
            buffer[pos+2] = color.b;
            buffer[pos+3] = color.a;
        }
    }
}

void bresenham_line(unsigned char *buffer, int x0, int y0, int x1, int y1, Color color, int thickness) {
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    
    int sx = x0 < x1 ? 1 : -1;
    int sy = y0 < y1 ? 1 : -1;

    int err = dx - dy;

    int x = x0, y = y0;

    for(;;) {
        put_pixel(buffer, x, y, color, thickness);
        if (x == x1 && y == y1) {
            break;
        }
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x += sx;
        }
        if (e2 < dx) {
            err += dx;
            y += sy;
        }
    }
}

void midpoint_circle(unsigned char *buffer, int x0, int y0, float radius, bool fill, Color color, int thickness) {
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

        if (fill) {
            bresenham_line(buffer, (int)(x0 + x), (int)(y0 + y), (int)(x0 - x), (int)(y0 + y), color, 0);
            bresenham_line(buffer, (int)(x0 + x), (int)(y0 - y), (int)(x0 - x), (int)(y0 - y), color, 0);
            bresenham_line(buffer, (int)(x0 + y), (int)(y0 + x), (int)(x0 - y), (int)(y0 + x), color, 0);
            bresenham_line(buffer, (int)(x0 + y), (int)(y0 - x), (int)(x0 - y), (int)(y0 - x), color, 0);
        }
        else {
            put_pixel(buffer, (int)(x0 + x), (int)(y0 + y), color, thickness);
            put_pixel(buffer, (int)(x0 - x), (int)(y0 + y), color, thickness);
            put_pixel(buffer, (int)(x0 + x), (int)(y0 - y), color, thickness);
            put_pixel(buffer, (int)(x0 - x), (int)(y0 - y), color, thickness);
            put_pixel(buffer, (int)(x0 + y), (int)(y0 + x), color, thickness);
            put_pixel(buffer, (int)(x0 - y), (int)(y0 + x), color, thickness);
            put_pixel(buffer, (int)(x0 + y), (int)(y0 - x), color, thickness);
            put_pixel(buffer, (int)(x0 - y), (int)(y0 - x), color, thickness);
        }

        x += 1;
    }
}

void rectangle(unsigned char *buffer, int x0, int y0, int x1, int y1, bool fill, Color color, int thickness) {
    if (x0 > x1) {
        std::swap(x0, x1);
    }
    if (y0 > y1) {
        std::swap(y0, y1);
    }
    if (fill) {
        for (int x = x0; x <= x1; x++) {
            for (int y = y0; y <= y1; y++) {
                put_pixel(buffer, x, y, color, 0);
            }
        }
    }
    else {
        for (int x = x0; x <= x1; x++) {
            put_pixel(buffer, x, y0, color, thickness);
            put_pixel(buffer, x, y1, color, thickness);
        }
        for (int y = y0; y <= y1; y++) {
            put_pixel(buffer, x0, y, color, thickness);
            put_pixel(buffer, x1, y, color, thickness);
        }
    }
}

void flood_fill(unsigned char *buffer, int x, int y, Color color) {
    if (x < 0 || y < 0 || x >= WIDTH || y >= HEIGHT) {
        return;
    }
    int pos = 4 * (y * WIDTH + x);

    Color old_color = Color(&buffer[pos]);
    if (old_color == color) {
        return;
    }
    struct _Point {
        int x = 0;
        int y = 0;
        _Point(int _x, int _y) : x(_x), y(_y) {}
    };
    std::queue<_Point> q;
    q.emplace(x, y);
    while (!q.empty()) {
        _Point p = q.front();
        q.pop();
        pos = 4 * (p.y * WIDTH + p.x);
        if (p.x < 0 || p.y < 0 || p.x >= WIDTH || p.y >= HEIGHT || !(Color(&buffer[pos]) == old_color)) {
            continue;
        }
        else {
            buffer[pos]     = color.r;
            buffer[pos + 1] = color.g;
            buffer[pos + 2] = color.b;
            buffer[pos + 3] = color.a;
            q.emplace(p.x + 1, p.y);
            q.emplace(p.x - 1, p.y);
            q.emplace(p.x, p.y + 1);
            q.emplace(p.x, p.y - 1);
        }
    }
}
