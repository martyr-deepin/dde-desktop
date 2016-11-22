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
    bool insert(const QString &id);
    bool insert(int x, int y, const QString &id);
    bool move(const QString &id, int x, int y);
    bool move(const QStringList &selectedIds, const QString &id, int x, int y);
    bool remove(const QString &id);
    bool remove(int x, int y, const QString &id);
    bool remove(int64_t value, const QString &id);

    bool contains(const QString &id);
    QPoint position(const QString &id);
    QString id(int x, int y);

    QPoint emptyCoordPos() const;
    QPoint takeEmptyCoordPos();
    CoordValue emptyCoordValue() const;
    CoordValue takeEmptyCoordValue();

    void setCoordinateSize(int w, int h);

signals:
    void setConfig(const QString &group, const QString &key, const QVariant &value);
    void removeConfig(const QString &group, const QString &key);

protected:
    friend Singleton<GridManager>;

    GridManager();
    ~GridManager();

    QScopedPointer<GridManagerPrivate> d;
};
