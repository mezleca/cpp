#pragma once

#include "client.hpp"
#include "../parsers/legacy/legacy.hpp"
#include "../parsers/legacy/legacy_collection.hpp"

#include <string>
#include <filesystem>

class StableClient : public Client {
  public:
    explicit StableClient(std::filesystem::path location);
    ~StableClient() override;

    // impl
    std::string get_player_name() override;
    OsuCollection* get_collection(std::string_view name) override;
    bool add_collection(OsuCollection* collection) override;
    bool delete_collection(std::string_view name) override;
    bool update_collection() override;
    OsuBeatmap* get_beatmap(std::string md5) override;
    OsuBeatmap* get_beatmap_by_id(int id) override;
    OsuBeatmapSet* get_beatmapset(int id) override;

  private:
    std::filesystem::path m_location;

    OsuLegacyCollection m_stable_collection;
    OsuLegacyDatabase m_stable_database;
};
