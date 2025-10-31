#include "ui/window.hpp"
#include <cstdlib>
#include <QApplication>
#include <QWidget>
#include <QFile>
#include <qapplication.h>
#include <qwidget.h>

int main(int argc, char** argv) {
    MainWindow window;
    return window.setup(argc, argv);
}
