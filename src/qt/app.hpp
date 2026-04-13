#pragma once

#include <QObject>
#include <QStringList>
#include <qtmetamacros.h>
#include <qqmlintegration.h>

class AppController : public QObject {
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(int current_tab READ get_current_tab WRITE set_current_tab NOTIFY current_tab_changed)
    Q_PROPERTY(QStringList tabs READ get_tabs CONSTANT)

  public:
    explicit AppController(QObject* parent = nullptr);

    // getters
    int get_current_tab() const {
        return m_current_tab;
    }

    QStringList get_tabs() const {
        return m_tabs;
    }

    // setters
    Q_INVOKABLE void set_current_tab(int tab) {
        if (m_current_tab == tab) {
            return;
        }

        m_current_tab = tab;
        emit current_tab_changed();
    }

  private:
    int m_current_tab = 0;
    QStringList m_tabs = {"collections", "browse", "discover", "radio", "config", "status"};

  signals:
    void current_tab_changed();
};
