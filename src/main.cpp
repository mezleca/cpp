#include "reader/binary.hpp"
#include <iostream>

int main(int argc, char** argv) {
    if (argc == 1) {
        std::cout << "expected osu.db path\n";
        return 1;
    }

    auto location = argv[1];
    auto buffer = binary_reader::file(location);

    if (buffer == nullptr) {
        return 1;
    }

    int offset = 0;
    int version = binary_reader::read<int>(buffer.get(), &offset);

    std::cout << "legacy version: " << version << "\n";
    return 0;
}
