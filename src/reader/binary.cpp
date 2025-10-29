#include <cstdint>
#include <cstring>
#include <iostream>

#include "binary.hpp"

char* binary_reader::file(char* location) {
    FILE *fp = fopen(location, "rb");
    
    if (!fp) {
        std::cerr << "failed to read file from " << location << "\n";
        return nullptr;
    }

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    std::cout << "bytes: " << size << "\n";

    char *buffer = (char*)malloc(size);

    if (buffer == NULL) {
        std::cerr << "failed to allocate buffer\n"; 
        fclose(fp);
        return nullptr;
    }

    size_t read = fread(buffer, 1, size, fp);
    fclose(fp);

    if (read != (size_t)size) {
        free(buffer);
        std::cerr << "failed to read file...\n";
        return nullptr;
    }
    
    return buffer;
};

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
