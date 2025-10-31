#include "window.hpp"
#include "fmt/base.h"
#include "tabs.hpp"
#include <QFile>
#include <cstdio>
#include <qapplication.h>
#include <qboxlayout.h>
#include <qwidget.h>
#include <qobject.h>

// not an widget or anything, just a normal class
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

    // add base layout
    QVBoxLayout* main_layout = new QVBoxLayout();
    window.setLayout(main_layout);

    // add tab layout
    Tabs* tabs = new Tabs(&window);
    main_layout->addLayout(tabs);
    
    // add tab content
    QWidget* content_widget = new QWidget();
    main_layout->addWidget(content_widget);

    // setup tab change signal
    // @TOFIX: this force a new layout every time we change the selected tab
    // im not sure if qt allow this but maybe we can temp save the layout?
    // idk if thats a good idea but might test it later
    QObject::connect(tabs, &Tabs::on_selected_tab, [&, content_widget, tabs](QString tab_name) {
        fmt::println("selected tab: {}", tab_name.toStdString());

        Tab* tab = tabs->get_tab_content(tab_name);

        if (tab == nullptr) {
            fmt::println(stderr, "failed to get content from {}", tab_name.toStdString());
            return;
        }
        
        // remove old layout
        QLayout* old_layout = content_widget->layout();

        if (old_layout != nullptr) {
            clear_layout(old_layout);
        }
        
        // build and set new layout
        QVBoxLayout* new_layout = tab->build();
        content_widget->setLayout(new_layout);
    });

    return app.exec();
};
