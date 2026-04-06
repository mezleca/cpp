#pragma once

#include <memory>
#include <optional>
#include <string_view>
#include <unordered_map>
#include <vector>

enum class BeatmapStatus : int32_t {
    UKNOWN = 0,
    UNSUBMITTED,
    GRAVEYARD,
    WIP,
    PENDING,
    UNUSED,
    RANKED,
    APPROVED,
    QUALIFIED,
    LOVED
};

enum class Gamemode : int32_t { OSU = 0, TAIKO, CATCH, MANIA };

struct osu_collection {
    std::string name;
    std::vector<std::string> checksums;
};

struct osu_beatmap {
    std::string artist;
    std::string artist_unicode;
    std::string title;
    std::string title_unicode;
    std::string creator;
    std::string difficulty;
    std::string audio_file_name;
    std::string md5;
    std::string osu_file_name;
    BeatmapStatus status = BeatmapStatus::UKNOWN;
    int32_t hitcircle = 0;
    int32_t sliders = 0;
    int32_t spinners = 0;
    int64_t last_modification_time = 0;
    double approach_rate = 0.0;
    double circle_size = 0.0;
    double hp_drain = 0.0;
    double overall_difficulty = 0.0;
    double slider_velocity = 0.0;
    int32_t drain_time = 0;
    int32_t total_time = 0;
    std::optional<double> duration;
    int32_t audio_preview_time = 0;
    int32_t difficulty_id = 0;
    int32_t beatmap_id = 0;
    Gamemode mode = Gamemode::OSU;
};

struct osu_beatmapset {
    std::string artist;
    std::string artist_unicode;
    std::string title;
    std::string title_unicode;
    std::string creator;
    int beatmapset_id;
    std::vector<osu_beatmap*> beatmaps;
};

class Client {
  public:
    virtual ~Client() = default;

    // methods
    virtual std::string get_player_name() = 0;
    virtual osu_collection* get_collection(std::string_view name) = 0;
    virtual bool add_collection(osu_collection* collection) = 0;
    virtual bool delete_collection(std::string_view name) = 0;
    virtual bool update_collection() = 0;
    virtual osu_beatmap* get_beatmap(std::string md5) = 0;
    virtual osu_beatmap* get_beatmap_by_id(int id) = 0;
    virtual osu_beatmapset* get_beatmapset(int id) = 0;

  protected:
    std::unordered_map<std::string_view, std::unique_ptr<osu_collection>> m_collections;
    std::unordered_map<std::string, std::unique_ptr<osu_beatmap>> m_beatmaps;
    std::unordered_map<int, std::unique_ptr<osu_beatmapset>> m_beatmapsets;

    bool m_collection_dirty = false;
};
