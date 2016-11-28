/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#pragma once

#include <QObject>
#include <QMap>
#include <QSettings>
#include <QScopedPointer>

#include "../global/coorinate.h"
#include "../global/singleton.h"

class GridManagerPrivate;
class GridManager: public QObject, public Singleton<GridManager>
{
    Q_OBJECT
public:
    bool add(const QString &id);
    bool add(QPoint pos, const QString &id);
    bool move(const QStringList &selectedIds, const QString &id, int x, int y);
    bool remove(const QString &id);
    bool remove(int x, int y, const QString &id);
    bool remove(QPoint pos, const QString &id);

    bool clear();

    bool contains(const QString &id);
    QPoint position(const QString &id);
    QString id(int x, int y);
    bool isEmpty(int x, int y);

    bool autoAlign();
    void toggleAlign();
    void reAlign();

    void setCoordinateSize(int w, int h);

protected:
    friend Singleton<GridManager>;

    GridManager();
    ~GridManager();


    QScopedPointer<GridManagerPrivate> d;
};
