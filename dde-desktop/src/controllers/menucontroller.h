#ifndef MENUCONTROLLER_H
#define MENUCONTROLLER_H

#include <QObject>

class MenuController : public QObject
{
    Q_OBJECT
public:
    explicit MenuController(QObject *parent = 0);
    ~MenuController();

signals:

public slots:
};

#endif // MENUCONTROLLER_H
