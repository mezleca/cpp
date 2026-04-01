#include "stable.hpp"
#include "client/client.hpp"
#include "fmt/base.h"
#include "parsers/legacy/legacy_collection.hpp"

#include <algorithm>
#include <memory>

std::unique_ptr<osu_collection> make_collection(legacy_collection* collection) {
    return std::make_unique<osu_collection>(collection->name, collection->beatmap_md5);
}

std::unique_ptr<osu_beatmap> make_beatmap(legacy_beatmap beatmap) {
    return std::make_unique<osu_beatmap>(
        beatmap.artist, beatmap.artist_unicode, beatmap.title, beatmap.title_unicode, beatmap.creator,
        beatmap.difficulty, beatmap.audio_file_name, beatmap.md5, beatmap.osu_file_name, (BeatmapStatus)beatmap.status,
        beatmap.hitcircle, beatmap.sliders, beatmap.spinners, beatmap.last_modification_time, beatmap.approach_rate,
        beatmap.circle_size, beatmap.hp_drain, beatmap.overall_difficulty, beatmap.slider_velocity, beatmap.drain_time,
        beatmap.total_time, beatmap.duration, beatmap.audio_preview_time, beatmap.difficulty_id, beatmap.beatmap_id,
        (Gamemode)beatmap.mode);
}

stableClient::stableClient(std::filesystem::path base) : m_location(base) {
    using path = std::filesystem::path;

    path db_location = base / "osu!.db";
    path cl_location = base / "collection.db";

    if (!legacy_parser::parse(db_location, &m_stable_database)) {
        fmt::println("failed to parse {}", db_location.c_str());
    }

    if (!legacy_collection_parser::parse(cl_location, &m_stable_collection)) {
        fmt::println("failed to parse {}", cl_location.c_str());
    }
}

stableClient::~stableClient() {
}

std::string stableClient::get_player_name() {
    return m_stable_database.player_name;
}

osu_collection* stableClient::get_collection(std::string_view name) {
    auto cache_it = m_collections.find(name);

    if (cache_it != m_collections.end()) {
        return cache_it->second.get();
    }

    auto it = std::find_if(m_stable_collection.collections.begin(), m_stable_collection.collections.end(),
                           [name](const legacy_collection& c) { return c.name == name; });

    if (it == m_stable_collection.collections.end()) {
        return nullptr;
    }

    auto result = make_collection(&(*it));
    osu_collection* ptr = result.get();
    m_collections.emplace(it->name, std::move(result));

    return ptr;
}

bool stableClient::delete_collection(std::string_view name) {
    m_collections.erase(name);

    auto it = std::find_if(m_stable_collection.collections.begin(), m_stable_collection.collections.end(),
                           [name](const legacy_collection& c) { return c.name == name; });

    if (it != m_stable_collection.collections.end()) {
        m_stable_collection.collections.erase(it);
        return true;
    }

    return false;
}