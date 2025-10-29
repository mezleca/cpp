#pragma once
#include <cstring>
#include <memory>
#include <string>

namespace binary_reader {
    std::unique_ptr<char[]> file(char* location);

    template <typename T>
    T read(char* buffer, int *offset) {
        T result;
        std::memcpy(&result, buffer + *offset, sizeof(T));
        *offset += sizeof(T);
        return result;
    };
    
    int uleb128(char* buffer, int* offset);
    std::string osu_string(char* buffer, int* offset);
};