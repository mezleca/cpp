#include "legacy.hpp"
#include "../../utils/binary.hpp"

#include <algorithm>

bool legacy_collection_parser::parse(const std::string& location) {
    if (data == nullptr) {
        return false;
    }

    std::vector<uint8_t> buffer;

    if (!osu_binary::read_file_buffer(location, buffer)) {
        return false;
    }

    try {
        osu_legacy_collection temp;
        osu_binary::binary_cursor cursor;

        osu_binary::set_cursor(cursor, buffer);

        temp.version = osu_binary::read_i32(cursor);
        temp.collections_count = osu_binary::read_i32(cursor);

        if (temp.collections_count < 0) {
            throw std::runtime_error("invalid collections count");
        }

        temp.collections.clear();
        temp.collections.reserve(static_cast<size_t>(std::max(0, temp.collections_count)));

        for (int32_t i = 0; i < temp.collections_count; i++) {
            legacy_collection collection;

            collection.name = osu_binary::read_string(cursor);
            collection.beatmaps_count = osu_binary::read_i32(cursor);

            if (collection.beatmaps_count < 0) {
                throw std::runtime_error("invalid collection beatmaps count");
            }

            collection.beatmap_md5.clear();
            collection.beatmap_md5.reserve(static_cast<size_t>(std::max(0, collection.beatmaps_count)));

            for (int32_t j = 0; j < collection.beatmaps_count; j++) {
                collection.beatmap_md5.push_back(osu_binary::read_string(cursor));
            }

            temp.collections.push_back(std::move(collection));
        }

        *data = std::move(temp);
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool legacy_collection_parser::write() {
    if (data == nullptr || location.empty()) {
        return false;
    }

    std::vector<uint8_t> buffer;
    buffer.reserve(1024);

    data->collections_count = static_cast<int32_t>(data->collections.size());

    osu_binary::write_i32(buffer, data->version);
    osu_binary::write_i32(buffer, data->collections_count);

    for (auto& collection : data->collections) {
        collection.beatmaps_count = static_cast<int32_t>(collection.beatmap_md5.size());
        osu_binary::write_string(buffer, collection.name);
        osu_binary::write_i32(buffer, static_cast<int32_t>(collection.beatmap_md5.size()));

        for (const auto& checksum : collection.beatmap_md5) {
            osu_binary::write_string(buffer, checksum);
        }
    }

    if (!osu_binary::write_file_buffer(location, buffer)) {
        return false;
    }

    return true;
}
