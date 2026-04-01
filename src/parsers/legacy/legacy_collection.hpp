#pragma once

#include <cstdint>
#include <string>
#include <vector>

// === collection.db ===
struct legacy_collection {
    std::string name;
    int32_t beatmaps_count = 0;
    std::vector<std::string> beatmap_md5;
};

struct osu_legacy_collection {
    int32_t version = 0;
    int32_t collections_count = 0;
    std::vector<legacy_collection> collections;
};

namespace legacy_collection_parser {
    bool parse(const std::string location, osu_legacy_collection* data);
    bool write(const std::string location, osu_legacy_collection* data);
};
