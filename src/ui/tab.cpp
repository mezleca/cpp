#include "tab.hpp"

Tab::Tab(QString _name, QWidget* parent) : QVBoxLayout(parent), name(_name) {}
Tab::~Tab() {}