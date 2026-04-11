#include "client.hpp"
#include "utils/query.hpp"
#include "utils/binary.hpp"

Client::Client() {
// dispatch table for variable fields
#define X(type, name, ...)                                                                                             \
    for (auto var : {#name, ##__VA_ARGS__}) {                                                                          \
        beatmap_table[var][QueryOp::EQ] = [](const OsuBeatmap& s, std::string_view v) {                                \
            return binary::lower_if_possible(s.name) == binary::convert_to<type>(v);                                   \
        };                                                                                                             \
        beatmap_table[var][QueryOp::NEQ] = [](const OsuBeatmap& s, std::string_view v) {                               \
            return binary::lower_if_possible(s.name) != binary::convert_to<type>(v);                                   \
        };                                                                                                             \
        beatmap_table[var][QueryOp::GTE] = [](const OsuBeatmap& s, std::string_view v) {                               \
            return binary::lower_if_possible(s.name) >= binary::convert_to<type>(v);                                   \
        };                                                                                                             \
        beatmap_table[var][QueryOp::GT] = [](const OsuBeatmap& s, std::string_view v) {                                \
            return binary::lower_if_possible(s.name) > binary::convert_to<type>(v);                                    \
        };                                                                                                             \
        beatmap_table[var][QueryOp::LTE] = [](const OsuBeatmap& s, std::string_view v) {                               \
            return binary::lower_if_possible(s.name) <= binary::convert_to<type>(v);                                   \
        };                                                                                                             \
        beatmap_table[var][QueryOp::LT] = [](const OsuBeatmap& s, std::string_view v) {                                \
            return binary::lower_if_possible(s.name) < binary::convert_to<type>(v);                                    \
        };                                                                                                             \
    }
    QUERY_FIELDS_V
#undef X

// dispatch table for non variable fields
#define X(type, name, ...)                                                                                             \
    beatmap_table[#name][QueryOp::EQ] = [](const OsuBeatmap& s, std::string_view v) {                                  \
        return binary::lower_if_possible(s.name) == binary::convert_to<type>(v);                                       \
    };                                                                                                                 \
    beatmap_table[#name][QueryOp::NEQ] = [](const OsuBeatmap& s, std::string_view v) {                                 \
        return binary::lower_if_possible(s.name) != binary::convert_to<type>(v);                                       \
    };                                                                                                                 \
    beatmap_table[#name][QueryOp::GTE] = [](const OsuBeatmap& s, std::string_view v) {                                 \
        return binary::lower_if_possible(s.name) >= binary::convert_to<type>(v);                                       \
    };                                                                                                                 \
    beatmap_table[#name][QueryOp::GT] = [](const OsuBeatmap& s, std::string_view v) {                                  \
        return binary::lower_if_possible(s.name) > binary::convert_to<type>(v);                                        \
    };                                                                                                                 \
    beatmap_table[#name][QueryOp::LTE] = [](const OsuBeatmap& s, std::string_view v) {                                 \
        return binary::lower_if_possible(s.name) <= binary::convert_to<type>(v);                                       \
    };                                                                                                                 \
    beatmap_table[#name][QueryOp::LT] = [](const OsuBeatmap& s, std::string_view v) {                                  \
        return binary::lower_if_possible(s.name) < binary::convert_to<type>(v);                                        \
    };
    QUERY_FIELDS_NV
#undef X
}

std::vector<OsuBeatmap*> Client::search_beatmaps(OsuSearchData data) {
    std::vector<OsuBeatmap*> result;
    QueryState state;

    std::string a = boost::locale::to_lower(data.query);
    std::string_view normalized_query(a);

    ParsedQuery p_query = query::parse(normalized_query);
    fmt::println("{}", p_query.content);

    for (auto& it : m_beatmaps) {
        OsuBeatmap* beatmap = it.second.get();
        bool match_token = true;

        for (const auto& token : p_query.tokens) {
            auto t_it = beatmap_table.find(token.key);
            if (t_it == beatmap_table.end()) continue;

            if (t_it->second[token.op](*beatmap, token.value)) {
                match_token = true;
                break;
            }
        }

        if (!match_token) {
            continue;
        }

        if (beatmap->searchable.find(p_query.content) == std::string::npos) {
            continue;
        }

        result.push_back(beatmap);
    }

    return result;
}

long Client::beatmap_count() {
    return m_beatmaps.size();
}