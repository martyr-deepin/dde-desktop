#ifndef KEYEVENTMANAGER_H
#define KEYEVENTMANAGER_H

#include <QtCore>

class KeyEventManager : public QObject
{
    Q_OBJECT
public:
    explicit KeyEventManager(QObject *parent = 0);
    ~KeyEventManager();

signals:

public slots:
    void onKeyUpPressed();
    void onKeyDownPressed();
    void onKeyLeftPressed();
    void onKeyRightPressed();
};

#endif // KEYEVENTMANAGER_H
