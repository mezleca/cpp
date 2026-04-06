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
    osu_collection* get_collection(std::string_view name) override;
    bool add_collection(osu_collection* collection) override;
    bool delete_collection(std::string_view name) override;
    bool update_collection() override;
    osu_beatmap* get_beatmap(std::string md5) override;
    osu_beatmap* get_beatmap_by_id(int id) override;
    osu_beatmapset* get_beatmapset(int id) override;

  private:
    std::string m_location = "";

    osu_legacy_collection m_stable_collection;
    osu_legacy_database m_stable_database;
};
