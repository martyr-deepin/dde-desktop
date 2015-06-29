#ifndef APPCONTROLLER_H
#define APPCONTROLLER_H

#include "menucontroller.h"
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
};

#endif // APPCONTROLLER_H
