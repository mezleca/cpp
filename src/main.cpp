#include "client/stable.hpp"
#include <QtQml>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QUrl>
#include <qqml.h>

#ifdef QML_SOURCE_DIR
#include <QFileSystemWatcher>
#include <QTimer>
#include <QDir>
#endif

#define APP_URI "osu_stuff"

static void reload_qml_engine(QQmlApplicationEngine& engine, const QUrl& mainUrl) {
    engine.clearComponentCache();

    for (QObject* obj : engine.rootObjects()) {
        obj->deleteLater();
    }

    engine.load(mainUrl);
}

int main(int argc, char** argv) {
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    // register theme
    qmlRegisterSingletonType(QUrl("qrc:/Theme.qml"), APP_URI, 1, 0, "Theme");
    StableClient client("/mnt/osu");

// DEBUG: load from local file and update on changes
#ifdef QML_SOURCE_DIR
    const QString qml_dir = QStringLiteral(QML_SOURCE_DIR);
    const QUrl main_qml = QUrl::fromLocalFile(qml_dir + "/Main.qml");

    engine.addImportPath(QDir(qml_dir).absolutePath() + "/..");
    engine.load(main_qml);

    // watch the entire dir
    QFileSystemWatcher watcher;
    watcher.addPath(qml_dir);

    for (const QString& f : QDir(qml_dir).entryList({"*.qml"}, QDir::Files)) {
        watcher.addPath(qml_dir + "/" + f);
    }

    QTimer debounce;

    debounce.setSingleShot(true);
    debounce.setInterval(100);

    QObject::connect(&watcher, &QFileSystemWatcher::fileChanged, &debounce, qOverload<>(&QTimer::start));
    QObject::connect(&watcher, &QFileSystemWatcher::directoryChanged, &debounce, qOverload<>(&QTimer::start));

    QObject::connect(&debounce, &QTimer::timeout, [&]() {
        qDebug() << "[hot-reload] reloading qml...";
        reload_qml_engine(engine, main_qml);

        for (const QString& f : QDir(qml_dir).entryList({"*.qml"}, QDir::Files)) {
            watcher.addPath(qml_dir + "/" + f);
        }
    });
#else
    engine.loadFromModule(APP_URI, "Main");
#endif

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    return app.exec();
}
