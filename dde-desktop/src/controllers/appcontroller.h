#ifndef APPCONTROLLER_H
#define APPCONTROLLER_H

#include <QObject>

class AppController : public QObject
{
    Q_OBJECT
public:
    explicit AppController(QObject *parent = 0);
    ~AppController();

signals:

public slots:
};

#endif // APPCONTROLLER_H
