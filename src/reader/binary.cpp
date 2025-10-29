#include <cstdint>
#include <cstring>
#include <iostream>
#include <memory>
#include <string>

#include "binary.hpp"

std::unique_ptr<char[]> binary_reader::file(char* location) {
    FILE* fp = fopen(location, "rb");

    if (!fp) {
        std::cerr << "failed to read file from " << location << "\n";
        return nullptr;
    }

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
 
    std::unique_ptr<char[]> buffer(new char[size]);

    size_t read = fread(buffer.get(), 1, size, fp);
    fclose(fp);

    if (read != (size_t)size) {
        std::cerr << "failed to read file...\n";
        return nullptr;
    }
    
    return buffer;
}

int binary_reader::uleb128(char *buffer, int *offset) {
    int shift = 0;
    int result = 0;

    while (true) {
        uint8_t b = read<uint8_t>(buffer, offset);
        result |= (b & 0x7F) << shift;
        shift += 7;

        if ((b & 0x80) == 0) {
            break;
        }
    }

    return result;
};

std::string binary_reader::osu_string(char* buffer, int* offset) {
    bool present = read<uint8_t>(buffer, offset);

    if (!present) {
        return "";
    }

    int size = uleb128(buffer, offset);
    std::string test(buffer, size);
    *offset += size;

    return test;
};