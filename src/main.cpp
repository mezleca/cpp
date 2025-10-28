#include <iostream>
#include <gui.hpp>

int main() {
    GUI gui;

    srand(time(0));

    if (!gui.init()) {
        std::cerr << "failed to initialize imgui\n";
        return -1;
    }

    // render loop
    while (!gui.should_close()) {
        gui.process_input();
        gui.render();
    }

    // TOFIX: segmentation fault on cleanup
    gui.cleanup();

    return 0;
}
