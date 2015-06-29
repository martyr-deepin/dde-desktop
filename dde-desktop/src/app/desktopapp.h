#ifndef DESKTOPAPP_H
#define DESKTOPAPP_H

#include <QObject>
#include "views/desktopframe.h"
#include "controllers/appcontroller.h"

class DesktopApp : public QObject
{
    Q_OBJECT
public:
    explicit DesktopApp(QObject *parent = 0);
    ~DesktopApp();

signals:

public slots:
    void show();

private:
    DesktopFrame* m_desktopFrame;
    AppController* m_appController;
};

#endif // DESKTOPAPP_H
