#ifndef APPCONTROLLER_H
#define APPCONTROLLER_H

#include "menucontroller.h"
#include "dbuscontroller.h"
#include <QObject>

class AppController : public QObject
{
    Q_OBJECT
public:
    explicit AppController(QObject *parent = 0);
    ~AppController();

    void initConnect();

signals:

public slots:



private:
    MenuController* m_menuController;
    DBusController* m_dbusController;
};

#endif // APPCONTROLLER_H
