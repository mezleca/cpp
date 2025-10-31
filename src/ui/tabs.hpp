#pragma once

#include "tab.hpp"

class Tabs : public QHBoxLayout {
    Q_OBJECT

private:
    QList<Tab*> tabs;
    QString active_tab;
    
private slots:
    void on_button_click();

signals:
    void on_selected_tab(QString name);

public:
    explicit Tabs(QWidget* parent);
    ~Tabs();

    Tab* get_tab_content(QString name);
    QString get_active_tab();
    void set_active_tab(int index);
};
