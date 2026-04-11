#pragma once

#include "boost/locale/conversion.hpp"
#include "parsers/legacy/legacy.hpp"
#include "../utils/query.hpp"
#include <fmt/format.h>

#include <boost/locale.hpp>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

enum class BeatmapStatus : int {
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

enum class Gamemode : int { OSU = 0, TAIKO, CATCH, MANIA };

struct OsuCollection {
    std::string name;
    std::vector<std::string> checksums;
};

// queryable fields with variations
#define QUERY_FIELDS_V                                                                                                 \
    X(double, approach_rate, "ar")                                                                                     \
    X(double, circle_size, "cs")                                                                                       \
    X(double, overall_difficulty, "od")                                                                                \
    X(double, hp_drain, "hp")

// queryable fields with no variations
#define QUERY_FIELDS_NV                                                                                                \
    X(std::string, artist)                                                                                             \
    X(std::string, title)                                                                                              \
    X(std::string, creator)                                                                                            \
    X(std::string, difficulty)                                                                                         \
    X(Gamemode, mode)                                                                                                  \
    X(BeatmapStatus, status)

// TODO: fields that requires custom dispatches like: played, key / keys, etc...

#define QUERY_FIELDS                                                                                                   \
    QUERY_FIELDS_NV                                                                                                    \
    QUERY_FIELDS_V

struct OsuBeatmap {
    explicit OsuBeatmap(const LegacyBeatmap& b)
        : artist(b.artist), artist_unicode(b.artist_unicode), title(b.title), title_unicode(b.title_unicode),
          creator(b.creator), difficulty(b.difficulty), audio_file_name(b.audio_file_name), md5(b.md5),
          osu_file_name(b.osu_file_name), status((BeatmapStatus)b.status), hitcircle(b.hitcircle), sliders(b.sliders),
          spinners(b.spinners), last_modification_time(b.last_modification_time), approach_rate(b.approach_rate),
          circle_size(b.circle_size), hp_drain(b.hp_drain), overall_difficulty(b.overall_difficulty), tags(b.tags),
          slider_velocity(b.slider_velocity), drain_time(b.drain_time), total_time(b.total_time), duration(b.duration),
          audio_preview_time(b.audio_preview_time), difficulty_id(b.difficulty_id), beatmap_id(b.beatmap_id),
          source(b.source), mode((Gamemode)b.mode) {
    }
    std::string searchable;
    std::string artist_unicode;
    std::string title_unicode;
    std::string audio_file_name;
    std::string md5;
    std::string source;
    std::string osu_file_name;
    std::string tags;
#define X(type, name, ...) type name;
    QUERY_FIELDS
#undef X
    int64_t last_modification_time = 0;
    double slider_velocity = 0.0;
    int hitcircle = 0;
    int sliders = 0;
    int spinners = 0;
    int drain_time = 0;
    int total_time = 0;
    std::optional<double> duration;
    int audio_preview_time = 0;
    int difficulty_id = 0;
    int beatmap_id = 0;

    void build_search() {
        searchable = boost::locale::to_lower(fmt::format("{} {} {} {} {} {} {} {} {} {}", title, title_unicode, artist,
                                                         artist_unicode, creator, difficulty, source, tags,
                                                         difficulty_id, beatmap_id));

        using nm = boost::locale::norm_type;
        searchable = boost::locale::normalize(searchable, nm::norm_nfd);
    }
};

struct OsuBeatmapSet {
    std::string artist;
    std::string artist_unicode;
    std::string title;
    std::string title_unicode;
    std::string creator;
    int beatmapset_id;
    std::vector<OsuBeatmap*> beatmaps;
};

struct OsuSearchData {
    std::string query;
    std::string sort; // TODO
};

class Client {
  public:
    Client();
    virtual ~Client() = default;

    // abstract methods
    virtual std::string get_player_name() = 0;
    virtual OsuCollection* get_collection(std::string_view name) = 0;
    virtual bool add_collection(OsuCollection* collection) = 0;
    virtual bool delete_collection(std::string_view name) = 0;
    virtual bool update_collection() = 0;
    virtual OsuBeatmap* get_beatmap(std::string md5) = 0;
    virtual OsuBeatmap* get_beatmap_by_id(int id) = 0;
    virtual OsuBeatmapSet* get_beatmapset(int id) = 0;

    // client methods
    std::vector<OsuBeatmap*> search_beatmaps(OsuSearchData data);
    long beatmap_count();

  protected:
    // shared data for osu related stuff
    std::unordered_map<std::string_view, std::unique_ptr<OsuCollection>> m_collections;
    std::unordered_map<std::string, std::unique_ptr<OsuBeatmap>> m_beatmaps;
    std::unordered_map<int, std::unique_ptr<OsuBeatmapSet>> m_beatmapsets;

    using dispatch_fn = bool (*)(const OsuBeatmap&, std::string_view);
    using beatmap_dispatch = std::unordered_map<QueryOp, dispatch_fn>;

    // dispatch table for query stuff
    std::unordered_map<std::string_view, beatmap_dispatch> beatmap_table;

    bool m_collection_dirty = false;
};
