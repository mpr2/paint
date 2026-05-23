#include <cmath>
#include <cstdio>
#include <cstdlib>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "paint.h"
#include "primitives.h"
#include "renderer.h"

struct State {
    GLFWwindow *window;
    unsigned char *screen;
    unsigned char *canvas;
    bool clear = false;
    bool drawing = false;
    int current_tool = 0;
    float color[4] = {0, 0, 0, 1};
    float draw_begin_x = 0;
    float draw_begin_y = 0;
    double mousex;
    double mousey;
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

    glfwGetCursorPos(window, &mousex, &mousey);

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && !ImGui::GetIO().WantCaptureMouse) {
        switch (current_tool) {
            case 0:
            case 1:
            if (!drawing) {
                draw_begin_x = (float)mousex;
                draw_begin_y = (float)mousey;
                drawing = true;
            }
        }
    }
    else {
        switch (current_tool) {
            case 0:
            if (drawing) {
                bresenham_line(canvas, (int)draw_begin_x, (int)draw_begin_y, (int)mousex, (int)mousey, color);
                drawing = false;
            }
            break;

            case 1:
            if (drawing) {
                float x = (float) fabs(mousex - draw_begin_x);
                float y = (float) fabs(mousey - draw_begin_y);
                midpoint_circle(canvas, draw_begin_x, draw_begin_y, sqrt(x*x + y*y), color);
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
            case 0:
            bresenham_line(screen, (int)draw_begin_x, (int)draw_begin_y, (int)mousex, (int)mousey, color);
            break;

            case 1:
            float x = (float) fabs(mousex - draw_begin_x);
            float y = (float) fabs(mousey - draw_begin_y);
            midpoint_circle(screen, draw_begin_x, draw_begin_y, sqrt(x*x + y*y), color);
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
    ImGui::SetNextItemWidth(200.0);
    ImGui::ColorPicker4("color", state->color, ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_AlphaBar);
    if (ImGui::Button("Clear")) {
        state->clear = true;
    }
    float x = ImGui::CalcTextSize("Circle").x;
    if (ImGui::Selectable("Line", state->current_tool == 0, 0, ImVec2(x, x))) {
        state->current_tool = 0;
    }
    ImGui::SameLine();
    if (ImGui::Selectable("Circle", state->current_tool == 1, 0, ImVec2(x, x))) {
        state->current_tool = 1;
    }
    ImGui::End();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
