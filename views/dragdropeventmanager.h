/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef DRAGDROPEVENTMANAGER_H
#define DRAGDROPEVENTMANAGER_H

#include <QtCore>
#include "desktopfwd.h"


class DragDropEventManager : public QObject
{
    Q_OBJECT
public:
    explicit DragDropEventManager(QObject *parent = 0);
    ~DragDropEventManager();

signals:

public slots:
    void handleDragMoveEvent(const QList<DesktopItemPointer>& items, const QList<QUrl>& urls, const QPoint& pos);
    void handleDropEvent(const QList<DesktopItemPointer>& items, QDropEvent* event);

private:
    DesktopItemPointer m_hoverDesktopItem = DesktopItemPointer(); /*desktop item which hover on*/
    DesktopItemPointer m_destinationDesktopItem = DesktopItemPointer(); /*desktop item which will drop*/
};

#endif // DRAGDROPEVENTMANAGER_H
