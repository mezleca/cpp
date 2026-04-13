#include "client/stable.hpp"
#include "fmt/base.h"

#include <QtQml>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QUrl>
#include <qqml.h>

#define APP_URI "osu_stuff"

int main(int argc, char** argv) {
    using retarded = std::chrono::steady_clock;

    boost::locale::generator gen;
    std::locale::global(gen(""));

    auto p_start = retarded::now();
    StableClient client{"/mnt/osu/"};

    fmt::println("took {} ms to initialize",
                 std::chrono::duration_cast<std::chrono::milliseconds>(retarded::now() - p_start).count());
    fmt::println("player name: {}", client.get_player_name());

    auto c = client.get_collection("good shit");
    auto c1 = client.get_collection("good shit");
    auto b1 = client.get_beatmapset(2066317);
    auto b2 = client.get_beatmap_by_id(4322862);

    fmt::println("c1 cached: name: {} | count: {}", c->name, c->checksums.size());
    fmt::println("c1 cached: name: {} | count: {}", c1->name, c1->checksums.size());

    if (b1) {
        fmt::println("beatmapset: {} - {} by {} ({} diffs)", b1->artist, b1->title, b1->creator, b1->beatmaps.size());
        fmt::println("1st diff of {}: {} {}", b1->title, b1->beatmaps[0]->difficulty,
                     b1->beatmaps[0]->overall_difficulty);
    }

    if (b2) {
        fmt::println("same diff but directly {}: {} (id: {})", b2->title, b2->difficulty, b2->difficulty_id);
    }

    if (client.delete_collection("good shit")) {
        fmt::println("removed collection");
    }

    if (client.delete_collection("good shit")) {
        fmt::println("removed collection 2");
    }

    auto s_start = retarded::now();
    std::vector search_result = client.search_beatmaps({.query = "Can't Say It Back (Feint Remix)"});

    // thats the cpp beaulty right here
    fmt::println("took {} ms to search through {} beatmaps and found {} beatmaps",
                 std::chrono::duration_cast<std::chrono::milliseconds>(retarded::now() - s_start).count(),
                 client.beatmap_count(), search_result.size());

    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    // register theme
    qmlRegisterSingletonType(QUrl("qrc:/Theme.qml"), APP_URI, 1, 0, "Theme");

    // load main qml
    engine.loadFromModule(APP_URI, "Main");

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    return app.exec();
}
