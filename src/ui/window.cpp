#include "window.hpp"
#include "fmt/base.h"
#include "tabs.hpp"
#include <QFile>
#include <qapplication.h>
#include <qboxlayout.h>
#include <qwidget.h>

MainWindow::MainWindow() {};

int MainWindow::setup(int argc, char** argv) {
    QApplication app(argc, argv);

    QString qss_location = ":/styles/main.qss";
    QFile qss_file(qss_location);

    if (!qss_file.open(QFile::ReadOnly)) {
        fmt::println(stderr, "failed to find {}!", qss_location.toStdString());
    }

    app.setStyleSheet(qss_file.readAll());

    QWidget window;
    window.setWindowTitle("osu-stuff");
    window.resize(800, 600);
    window.show();

    // add base layoutu
    QVBoxLayout* main_layout = new QVBoxLayout();
    window.setLayout(main_layout);

    // add tabs to layout
    Tabs* tabs = new Tabs(&window);
    main_layout->addLayout(tabs);

    return app.exec();
};
