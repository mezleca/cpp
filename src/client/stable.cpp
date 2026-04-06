#include "stable.hpp"
#include "client/client.hpp"
#include "fmt/base.h"
#include "fmt/format.h"
#include "parsers/legacy/legacy_collection.hpp"

#include <algorithm>
#include <memory>
#include <stdexcept>

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

std::unique_ptr<osu_beatmapset> make_beatmapset(osu_beatmap* beatmap) {
    std::vector<osu_beatmap*> beatmaps;
    return std::make_unique<osu_beatmapset>(beatmap->artist, beatmap->artist_unicode, beatmap->title,
                                            beatmap->title_unicode, beatmap->creator, beatmap->beatmap_id, beatmaps);
}

StableClient::StableClient(std::filesystem::path base) : m_location(base) {
    using path = std::filesystem::path;

    path db_location = base / "osu!.db";
    path cl_location = base / "collection.db";

    if (!legacy_parser::parse(db_location, &m_stable_database)) {
        throw std::runtime_error(fmt::format("failed to parse {}", db_location.c_str()));
    }

    if (!legacy_collection_parser::parse(cl_location, &m_stable_collection)) {
        throw std::runtime_error(fmt::format("failed to parse {}", cl_location.c_str()));
    }

    // build beatmaps / beatmapsets
    for (auto& raw_beatmap : m_stable_database.beatmaps) {
        std::string md5 = raw_beatmap.md5;
        int beatmapset_id = raw_beatmap.beatmap_id;

        auto beatmap = make_beatmap(raw_beatmap);
        auto [it, _] = m_beatmaps.emplace(md5, std::move(beatmap));
        auto beatmap_ptr = it->second.get();

        if (m_beatmapsets.find(beatmapset_id) == m_beatmapsets.end()) {
            auto new_set = make_beatmapset(beatmap_ptr);
            new_set->beatmaps.emplace_back(beatmap_ptr);
            m_beatmapsets.emplace(beatmapset_id, std::move(new_set));
            continue;
        }

        auto& set = m_beatmapsets.at(beatmapset_id);
        set->beatmaps.emplace_back(beatmap_ptr);
    }
}

StableClient::~StableClient() {
}

std::string StableClient::get_player_name() {
    return m_stable_database.player_name;
}

bool StableClient::add_collection(osu_collection* collection) {
    auto [_, success] = m_collections.try_emplace(std::string_view(collection->name), collection);
    return success;
}

osu_collection* StableClient::get_collection(std::string_view name) {
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

bool StableClient::delete_collection(std::string_view name) {
    m_collections.erase(name);

    auto it = std::find_if(m_stable_collection.collections.begin(), m_stable_collection.collections.end(),
                           [name](const legacy_collection& c) { return c.name == name; });

    if (it != m_stable_collection.collections.end()) {
        m_stable_collection.collections.erase(it);
        return true;
    }

    return false;
}

bool StableClient::update_collection() {
    if (!m_collection_dirty)
        return true;

    for (auto& collection : m_stable_collection.collections) {
        auto& modified = m_collections.at(collection.name);

        collection.name = modified->name;
        collection.beatmap_md5 = modified->checksums;
    }

    m_collection_dirty = false;
    return true;
}

osu_beatmap* StableClient::get_beatmap(std::string md5) {
    if (m_beatmaps.find(md5) == m_beatmaps.end())
        return nullptr;
    auto& beatmap = m_beatmaps.at(md5);
    return beatmap.get();
}

osu_beatmap* StableClient::get_beatmap_by_id(int id) {
    for (const auto& [_, beatmap] : m_beatmaps) {
        if (beatmap->difficulty_id == id)
            return beatmap.get();
    }

    return nullptr;
}

osu_beatmapset* StableClient::get_beatmapset(int id) {
    if (m_beatmapsets.find(id) == m_beatmapsets.end())
        return nullptr;
    auto& beatmapset = m_beatmapsets.at(id);
    return beatmapset.get();
}
