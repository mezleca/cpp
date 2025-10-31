#include "tabs.hpp"
#include "tabs/test.hpp"
#include <qpushbutton.h>
#include <iostream>

Tabs::Tabs(QWidget* parent) : QHBoxLayout(parent) {
    // setup tabs
    TestTab* test_tab = new TestTab(parent);
    tabs.append(test_tab);

    // setup slots
    for (auto& t : tabs) {
        // create tab button
        QPushButton *btn = new QPushButton(t->get_tab_name());
        btn->setParent(nullptr);

        // update active tab on click
        connect(btn, &QPushButton::clicked, this, &Tabs::on_button_click);

        // add button to layout
        addWidget(btn);
    }
};

void Tabs::on_button_click() {
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    std::cout << button->text().toStdString() << "\n";
}

Tabs::~Tabs() {};