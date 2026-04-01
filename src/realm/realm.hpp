#pragma once

#include "./lazer.hpp"

namespace realmdb {
    inline realm::db open_lazer(const std::string& path) {
        realm::db_config config;
        config.set_path(path);
        config.set_schema_mode(realm::db_config::schema_mode::additive_discovered);

        return realm::open<realm::BeatmapDifficulty, realm::BeatmapMetadata, realm::BeatmapUserSettings,
                           realm::RealmUser, realm::Ruleset, realm::File, realm::RealmNamedFileUsage,
                           realm::BeatmapCollection, realm::BeatmapSet, realm::Beatmap>(config);
    }
}
