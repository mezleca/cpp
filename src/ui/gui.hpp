#pragma once

#include "imgui_internal.h"
#include <GLFW/glfw3.h>

class GUI {
private:
    ImGuiContext* ctx;
    GLFWwindow* window;
    bool running;
public:
    GUI();

    bool init();
    void cleanup();
    bool should_close() const;
    void process_input();
    void render();
};