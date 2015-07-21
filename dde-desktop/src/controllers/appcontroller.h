#ifndef APPCONTROLLER_H
#define APPCONTROLLER_H

#include "menucontroller.h"
#include "movejobcontroller.h"
#include "trashjobcontroller.h"
#include "copyjobcontroller.h"
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
    TrashJobController* m_trashJobController;
    MoveJobController* m_moveJobController;
    CopyJobController* m_copyJobController;
    DBusController* m_dbusController;
};

#endif // APPCONTROLLER_H
