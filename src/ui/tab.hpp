#pragma once

#include <qboxlayout.h>
#include <qlayout.h>
#include <qtmetamacros.h>

class Tab : public QVBoxLayout {
    Q_OBJECT
private:
    QVBoxLayout* layout;
    QString name;
public:
    explicit Tab(QString _name, QWidget* parent = nullptr);
    ~Tab();

    QString get_tab_name() { return name; }
    QLayout* get_layout() { return layout; }

    virtual QVBoxLayout* build() = 0;
};