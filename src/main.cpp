#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <thread>
#include "ui/gui.hpp"
#include "threading/pool.hpp"

int main() {
    GUI gui;

    srand(time(NULL));
    pool.initialize(std::max((int)std::thread::hardware_concurrency(), 1));

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
