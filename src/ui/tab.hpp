#pragma once

#include <qboxlayout.h>
#include <qtmetamacros.h>

class Tab : public QVBoxLayout {
    Q_OBJECT
private:
    QString name;
public:
    Tab(QString _name, QWidget* parent = nullptr);
    ~Tab();

    QString get_tab_name() { return name; }
};
