#ifndef KEYEVENTMANAGER_H
#define KEYEVENTMANAGER_H

#include <QtCore>
#include <QtGui>
#include <QtWidgets>
#include "desktopitem.h"


class KeyEventManager : public QObject
{
    Q_OBJECT
public:
    explicit KeyEventManager(QWidget *parent = 0);
    ~KeyEventManager();

    void initConnect();

    void clearMultiCheckedByMouse();


signals:

public slots:
    void onKeyUpPressed();
    void onKeyDownPressed();
    void onKeyLeftPressed();
    void onKeyRightPressed();

    void onKeyShiftUpPressed();
    void onKeyShiftDownPressed();
    void onKeyShiftLeftPressed();
    void onKeyShiftRightPressed();

};

#endif // KEYEVENTMANAGER_H
