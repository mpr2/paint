#pragma once

#include "paint.h"

void put_pixel(unsigned char *canvas, int x, int y, float color[4]);
void bresenham_line(unsigned char *canvas, int p0x, int p0y, int p1x, int p1y, float color[4]);
void midpoint_circle(unsigned char *canvas, float x0, float y0, float radius, float color[4]);
