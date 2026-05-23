#pragma once

#include <glad/glad.h>

struct Renderer {
    GLuint vao;
    GLuint pos_buffer;
    GLuint tex_buffer;
    GLuint ebo;
    GLuint tex_screen;
    GLuint tex_canvas;
    GLuint shader_program;

    void setup_input();
    void compile_shaders();
    void draw(unsigned char *screen, unsigned char *canvas);
};
