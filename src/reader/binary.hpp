#pragma once
#include <cstring>
#include <string>

namespace binary_reader {
    char* file(char* location);

    template <typename T>
    T read(char* buffer, int *offset) {
        T result;
        std::memcpy(&result, buffer + *offset, sizeof(T));
        *offset += sizeof(T);
        return result;
    };
    
    int uleb128(char* buffer, int* offset);
    std::string str();
};