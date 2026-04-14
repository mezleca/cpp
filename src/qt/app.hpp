#pragma once

#include <QObject>
#include <QStringList>
#include <qtmetamacros.h>
#include <qqmlintegration.h>

class AppController : public QObject {
    Q_OBJECT
    QML_ELEMENT

  public:
    explicit AppController(QObject* parent = nullptr);

  private:
};
