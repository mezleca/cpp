#include "tabs.hpp"
#include "tabs/test.hpp"
#include <qpushbutton.h>

Tabs::Tabs(QWidget* parent) : QHBoxLayout(parent) {
    // setup tabs
    tabs.append(new TestTab(parent, "a"));
    tabs.append(new TestTab(parent, "b"));
    tabs.append(new TestTab(parent, "c"));

    // setup slots
    for (auto& t : tabs) {
        // create tab button
        QPushButton *btn = new QPushButton(t->get_tab_name());

        // update active tab on click
        connect(btn, &QPushButton::clicked, this, &Tabs::on_button_click);

        // add button to layout
        addWidget(btn);
    }
};

void Tabs::on_button_click() {
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    QString selected_tab = button->text();

    if (active_tab == selected_tab) {
        return;
    }
    
    active_tab = selected_tab;
    emit on_selected_tab(active_tab);
};

void Tabs::set_active_tab(int index) {
    if (index >= 0 && index < tabs.size()) {
        active_tab = tabs[index]->get_tab_name();
        emit on_selected_tab(active_tab);
    }
};

Tab* Tabs::get_tab_content(QString name) {
    for (auto& tab : tabs) {
        if (tab->get_tab_name() == name) {
            return tab;
        }
    }

    return nullptr;
};

QString Tabs::get_active_tab() {
    return active_tab;
}

Tabs::~Tabs() {
    for (auto& tab : tabs) {
        delete tab;
    }
}