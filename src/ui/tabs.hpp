#pragma once

#include "tab.hpp"

class Tabs : public QHBoxLayout {
    Q_OBJECT
private:
    QString* active_tab;
    QList<Tab*> tabs;
private slots:
    void on_button_click();
public:
    Tabs(QWidget* parent);
    ~Tabs();
};
