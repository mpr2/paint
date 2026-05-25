#pragma once

#include "paint.h"

struct Color {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;

    Color(unsigned char _r, unsigned char _g, unsigned char _b, unsigned char _a)
        : r(_r), g(_g), b(_b), a(_a) {}

    Color(float c[4]) {
        r = (unsigned char) (c[0] * 255);
        g = (unsigned char) (c[1] * 255);
        b = (unsigned char) (c[2] * 255);
        a = (unsigned char) (c[3] * 255);
    }

    Color(unsigned char c[4]) {
        r = c[0];
        g = c[1];
        b = c[2];
        a = c[3];
    }

    bool operator== (Color c) {
        return (r == c.r && g == c.g &&
                b == c.b && a == c.a);
    }
};

void put_pixel(unsigned char *buffer, int x, int y, Color color, int thickness);
void bresenham_line(unsigned char *buffer, int p0x, int p0y, int p1x, int p1y, Color color, int thickness);
void midpoint_circle(unsigned char *buffer, int x0, int y0, float radius, bool fill, Color color, int thickness);
void rectangle(unsigned char *buffer, int x0, int y0, int x1, int y1, bool fill, Color color, int thickness);
void flood_fill(unsigned char *buffer, int x, int y, Color color);
