#include <cmath>
#include <cstdio>
#include <cstdlib>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "paint.h"
#include "primitives.h"
#include "renderer.h"

struct State {
    GLFWwindow *window;
    unsigned char *screen;
    unsigned char *canvas;
    bool clear = false;
    bool drawing = false;
    int current_tool = TOOL_PENCIL;
    Color color = Color(0,0,0,255);
    int thickness = 0;
    bool fill = false;
    int draw_begin_x = 0;
    int draw_begin_y = 0;
    int mousex;
    int mousey;
    double last_frame_time = 0;
    double delta_time;

    void update();
};

GLFWwindow* initialize_glfw();
void initialize_imgui(GLFWwindow *window);
void draw_imgui(State *state);


int main() {
    GLFWwindow *window = initialize_glfw();

    // buffer for drawing previews on top of the canvas
    unsigned char *screen = new unsigned char[BUFSIZE];
    // buffer where the drawing will be stored
    unsigned char *canvas = new unsigned char[BUFSIZE];
    memset(canvas, 0, BUFSIZE);

    glViewport(0, 0, WIDTH, HEIGHT);
    glfwSwapInterval(0); // uncap framerate

    initialize_imgui(window);

    Renderer renderer;
    renderer.setup_input();
    renderer.compile_shaders();

    State state;
    state.window = window;
    state.screen = screen;
    state.canvas = canvas;

    while (!glfwWindowShouldClose(window)) {
        state.update();

        renderer.draw(screen, canvas);

        draw_imgui(&state);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    delete[] screen;
    delete[] canvas;

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();

    return 0;
}

GLFWwindow* initialize_glfw() {
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        exit(-1);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, 0);

    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "paint", NULL, NULL);
    if (window == NULL) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        exit(-1);
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        fprintf(stderr, "Failed to initialize GLAD\n");
        glfwTerminate();
        exit(-1);
    }

    return window;
}

void initialize_imgui(GLFWwindow *window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");
}

void State::update() {
    double t = glfwGetTime();
    delta_time = t - last_frame_time;
    last_frame_time = t;

    double mx, my;
    glfwGetCursorPos(window, &mx, &my);
    mousex = (int) mx;
    mousey = (int) my;

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && !ImGui::GetIO().WantCaptureMouse) {
        switch (current_tool) {
            case TOOL_LINE:
            case TOOL_CIRCLE:
            case TOOL_PENCIL:
            case TOOL_ERASER:
            case TOOL_RECT:
            if (!drawing) {
                draw_begin_x = mousex;
                draw_begin_y = mousey;
                drawing = true;
            }
            break;

            case TOOL_BUCKET:
            if (!drawing) {
                flood_fill(canvas, mousex, mousey, color);
                drawing = true;
            }
        }
    }
    else {
        switch (current_tool) {
            case TOOL_LINE:
            if (drawing) {
                bresenham_line(canvas, draw_begin_x, draw_begin_y, mousex, mousey, color, thickness);
                drawing = false;
            }
            break;

            case TOOL_CIRCLE:
            if (drawing) {
                float x = (float) fabs(mousex - draw_begin_x);
                float y = (float) fabs(mousey - draw_begin_y);
                midpoint_circle(canvas, draw_begin_x, draw_begin_y, sqrt(x*x + y*y), fill, color, thickness);
                drawing = false;
            }
            break;

            case TOOL_PENCIL:
            case TOOL_ERASER:
            case TOOL_BUCKET:
            drawing = false;
            break;

            case TOOL_RECT:
            if (drawing) {
                rectangle(canvas, draw_begin_x, draw_begin_y, mousex, mousey, fill, color, thickness);
                drawing = false;
            }
        }
    }

    memset(screen, 0, BUFSIZE);
    if (clear) {
        memset(canvas, 0, BUFSIZE);
        clear = false;
    }

    if (drawing) {
        switch(current_tool) {
            case TOOL_LINE: {
                bresenham_line(screen, draw_begin_x, draw_begin_y, mousex, mousey, color, thickness);
                break;
            }

            case TOOL_CIRCLE: {
                float x = (float) fabs(mousex - draw_begin_x);
                float y = (float) fabs(mousey - draw_begin_y);
                midpoint_circle(screen, draw_begin_x, draw_begin_y, sqrt(x*x + y*y), fill, color, thickness);
                break;
            }

            case TOOL_PENCIL: {
                bresenham_line(canvas, draw_begin_x, draw_begin_y, mousex, mousey, color, thickness);
                draw_begin_x = mousex;
                draw_begin_y = mousey;
                break;
            }
            case TOOL_ERASER: {
                float c[4] = {0,0,0,0};
                bresenham_line(canvas, draw_begin_x, draw_begin_y, mousex, mousey, c, thickness + 2);
                draw_begin_x = mousex;
                draw_begin_y = mousey;
                break;
            }
            case TOOL_BUCKET: {
                break;
            }
            case TOOL_RECT: {
                rectangle(screen, draw_begin_x, draw_begin_y, mousex, mousey, fill, color, thickness);
            }
        }
    }
}

void draw_imgui(State *state) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("paint");
    ImGui::Text("Frametime: %f ms", 1000*state->delta_time);
    ImGui::Text("FPS: %.0f", 1/state->delta_time);

    if (ImGui::Button("Clear")) {
        state->clear = true;
    }

    if (ImGui::Button("Save")) {
        stbi_write_png("image.png", WIDTH, HEIGHT, 4, state->canvas, 4*WIDTH);
    }

    ImGui::SliderInt("Thickness", &state->thickness, 0, 10);
    float c[4] = {
        state->color.r / 255.0f,
        state->color.g / 255.0f,
        state->color.b / 255.0f,
        state->color.a / 255.0f,
    };

    if (ImGui::ColorEdit4("color", c, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar)) {
        state->color = Color(c);
    }

    ImGui::Checkbox("Fill shape", &state->fill);

    float x = ImGui::CalcTextSize("Circle").x;
    if (ImGui::Selectable("Pencil", state->current_tool == TOOL_PENCIL, 0, ImVec2(x, x))) {
        state->current_tool = TOOL_PENCIL;
    }
    ImGui::SameLine();
    if (ImGui::Selectable("Eraser", state->current_tool == TOOL_ERASER, 0, ImVec2(x, x))) {
        state->current_tool = TOOL_ERASER;
    }
    ImGui::SameLine();
    if (ImGui::Selectable("Bucket", state->current_tool == TOOL_BUCKET, 0, ImVec2(x, x))) {
        state->current_tool = TOOL_BUCKET;
    }
    if (ImGui::Selectable("Line", state->current_tool == TOOL_LINE, 0, ImVec2(x, x))) {
        state->current_tool = TOOL_LINE;
    }
    ImGui::SameLine();
    if (ImGui::Selectable("Circle", state->current_tool == TOOL_CIRCLE, 0, ImVec2(x, x))) {
        state->current_tool = TOOL_CIRCLE;
    }
    ImGui::SameLine();
    if (ImGui::Selectable("Rect", state->current_tool == TOOL_RECT, 0, ImVec2(x, x))) {
        state->current_tool = TOOL_RECT;
    }

    ImGui::End();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
