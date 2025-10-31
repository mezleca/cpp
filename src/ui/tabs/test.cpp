#include "test.hpp"
#include <qboxlayout.h>
#include <qlabel.h>

TestTab::TestTab(QWidget* parent, QString name) : Tab(name, parent) {}

QVBoxLayout* TestTab::build() {
    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(new QLabel(get_tab_name()));

    return layout;
};

TestTab::~TestTab() {}