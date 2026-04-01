#include "client/stable.hpp"
#include "fmt/base.h"
#include "qt/app.hpp"

#include <QtQml>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QUrl>
#include <iostream>
#include <qqml.h>

#define APP_URI "osu_stuff"

int main(int argc, char** argv) {
    stableClient client{"/mnt/osu/"};

    fmt::println("player name: {}", client.get_player_name());

    auto c = client.get_collection("good shit");
    auto c1 = client.get_collection("good shit");

    fmt::println("c1 cached: name: {} | count: {}", c->name, c->checksums.size());
    fmt::println("c1 cached: name: {} | count: {}", c1->name, c1->checksums.size());

    if (client.delete_collection("good shit")) {
        fmt::println("removed collection");
    }

    if (client.delete_collection("good shit")) {
        fmt::println("removed collection 2");
    }

    return 0;

    // QGuiApplication app(argc, argv);
    // QQmlApplicationEngine engine;

    // AppController app_controller;

    // // register theme
    // qmlRegisterSingletonType(QUrl("qrc:/Theme.qml"), APP_URI, 1, 0, "Theme");

    // // expose controller to qml
    // engine.rootContext()->setContextProperty("app", &app_controller);

    // // load main qml
    // engine.loadFromModule(APP_URI, "Main");

    // if (engine.rootObjects().isEmpty()) {
    //     return -1;
    // }

    // return app.exec();
}
