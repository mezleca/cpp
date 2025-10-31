#pragma once
#include "../tab.hpp"
#include <qwidget.h>

class TestTab : public Tab {
private:
public:
    TestTab(QWidget* parent = nullptr);
    ~TestTab();
};