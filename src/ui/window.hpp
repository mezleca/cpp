#pragma once
#include <QMainWindow>
#include <qlayoutitem.h>
#include <qwidget.h>
#include "QBoxLayout"

class MainWindow {
private:
    void clear_layout(QLayout* layout) {
        QLayoutItem* item;
        while ((item = layout->takeAt(0)) != nullptr) {
            if (item->widget()) {
                delete item->widget();
            }
            delete item;
        }

        delete layout;
    }
public:
    MainWindow();
    int setup(int argc, char** argv);
};
