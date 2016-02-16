/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef KEYEVENTMANAGER_H
#define KEYEVENTMANAGER_H

#include <QtCore>
#include "desktopfwd.h"


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
    void onKeyUpPressed(bool isMutilChecked=false);
    void onKeyDownPressed(bool isMutilChecked=false);
    void onKeyLeftPressed(bool isMutilChecked=false);
    void onKeyRightPressed(bool isMutilChecked=false);

    void onKeyShiftUpPressed();
    void onKeyShiftDownPressed();
    void onKeyShiftLeftPressed();
    void onKeyShiftRightPressed();

    void onKeyHomePressed();
    void onKeyEndPressed();

    void onKeyEnterPressed();

    void onKeyShiftHomePressed();
    void onKeyShiftEndPressed();

    void checkFirstDesktopItem();
    void checkLastDesktopItem();
private:
    DesktopItemPointer m_lastCheckedByKeyboard;

};

#endif // KEYEVENTMANAGER_H
