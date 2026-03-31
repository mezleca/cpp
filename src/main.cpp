#include "qt/app.hpp"

#include <QtQml>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QUrl>
#include <qqml.h>

#define APP_URI "osu_stuff"

int main(int argc, char** argv) {
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    AppController app_controller;

    // register theme
    qmlRegisterSingletonType(QUrl("qrc:/Theme.qml"), APP_URI, 1, 0, "Theme");

    // expose controller to qml
    engine.rootContext()->setContextProperty("app", &app_controller);

    // load main qml
    engine.loadFromModule(APP_URI, "Main");

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    return app.exec();
}
