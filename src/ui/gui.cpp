#include "cpr/api.h"
#include "cpr/cprtypes.h"
#include "cpr/response.h"
#include <cstdlib>
#include <glad/glad.h>
#include <memory>
#include <mutex>
#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include "gui.hpp"
#include "../threading/pool.hpp"
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <iostream>
#include <fmt/format.h>
#include <cpr/cpr.h>
#include "../reader/binary.hpp"

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define GLSL_VERSION "#version 130"

GUI::GUI() {}

bool GUI::init() {
    if (!glfwInit()) {
        std::cerr << "failed to initialze GLFW\n";
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Hello WOrld", nullptr, nullptr);
    if (!window) {
        std::cerr << "failed to create window\n";
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cerr << "failed to load glad" << "\n";
        return false;
    }

    IMGUI_CHECKVERSION();
    ctx = ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(GLSL_VERSION);

    running = true;
    return true;
}

void GUI::cleanup() {
    if (window) {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();

        if (ctx) {
            ImGui::DestroyContext(ctx);
            ctx = nullptr;
        }

        glfwDestroyWindow(window);
        glfwTerminate();
        window = nullptr;
    }
}

bool GUI::should_close() const {
    return !running || glfwWindowShouldClose(window);
}

void GUI::process_input() {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        running = false;
    }
    glfwPollEvents();
}

std::string get_test_data() {
    std::string url = fmt::format("https://jsonplaceholder.typicode.com/comments/{}", (int)rand() % 500);
    std::cout << fmt::format("fetching: {}", url) << "\n";
    cpr::Response res = cpr::Get(cpr::Url("https://jsonplaceholder.typicode.com/comments/3"));
    std::cout << fmt::format("raw_response:\n{}", res.text) << "\n";

    return res.text;
}

struct basic_data {
    int version;
    int count;
    bool account_unlocked;
    long unlocked_date;
    std::string player_name;
};

void GUI::render() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    static int counter = 0;
    static char location[256];
    static basic_data data;
    static std::string raw_output = "";
    static std::string data_status = "";
    static std::mutex data_mtx;

    ImGui::Begin("controls");
    {
        ImGui::Text("OpenGL: %s", glGetString(GL_VERSION));
        ImGui::Text("fps: %.1f", ImGui::GetIO().Framerate);
        ImGui::Separator();

        if (ImGui::Button("click")) counter++;
        ImGui::Text("counter: %d", counter);

        ImGui::Separator();
        ImGui::InputText("osu data", location, sizeof(location));

        if (data_status != "") {
            ImGui::Text("status: %s", data_status.c_str());
        }

        if (data.version) {
            ImGui::Text("version: %i", data.version);
            ImGui::Text("folder count: %i", data.count);
            ImGui::Text("unlocked: %s", data.account_unlocked ? "true" : "false");
            ImGui::Text("player: %s", data.player_name.c_str());
        }

        if (ImGui::Button("read")) {
            pool.enqueue([]() {
                {
                    std::lock_guard<std::mutex> lock(data_mtx);
                    auto buffer = binary_reader::file(location);

                    if (buffer == nullptr) {
                        data_status = "failed to read (invalid buffer)";
                        return;
                    }

                    int offset = 0;
                    data.version = binary_reader::read<int>(buffer.get(), &offset);
                    data.count = binary_reader::read<int>(buffer.get(), &offset);
                    data.account_unlocked = binary_reader::read<bool>(buffer.get(), &offset);
                    data.unlocked_date = binary_reader::read<long>(buffer.get(), &offset);
                    data.player_name = binary_reader::osu_string(buffer.get(), &offset);
                }
            });
        }

        if (ImGui::Button("do random request")) {
            pool.enqueue([]() {
                raw_output = get_test_data();
            });
        }

        if (!raw_output.empty()) {
            ImGui::Separator();
            ImGui::Text("raw data:");
            ImGui::Text("%s", raw_output.c_str());
            ImGui::Separator();
        }
    }    
    ImGui::End();

    ImGui::Render();
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    glViewport(0, 0, w, h);
    glClearColor(0.45f, 0.55f, 0.60f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
}
