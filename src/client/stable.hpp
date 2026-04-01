#pragma once

#include "client.hpp"
#include "../parsers/legacy/legacy.hpp"
#include "../parsers/legacy/legacy_collection.hpp"

#include <string>
#include <filesystem>

class stableClient : public Client {
  public:
    explicit stableClient(std::filesystem::path location);
    ~stableClient() override;

    std::string get_player_name() override;
    osu_collection* get_collection(std::string_view name) override;
    bool delete_collection(std::string_view name) override;

  private:
    std::string m_location = "";

    osu_legacy_collection m_stable_collection;
    osu_legacy_database m_stable_database;
};
