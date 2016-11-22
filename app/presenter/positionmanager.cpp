/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "positionmanager.h"

#include <QPoint>
#include <QDebug>

#include "../config/config.h"

static QString positionKey(int64_t value);
static int64_t positionValue(int x, int y);
static QPoint positionFromValue(int64_t);

QString positionKey(int64_t value)
{
    auto pos = Coordinate(value).position();
    return QString("%1_%2").arg(pos.x()).arg(pos.y());
}

int64_t positionValue(int x, int y)
{
    return Coordinate(x, y).value();
}

QPoint positionFromValue(int64_t v)
{
    return Coordinate(v).position();
}

class GridManagerPrivate
{
public:
    GridManagerPrivate()
    {
        auto settings = Config::instance()->settings();
        settings->beginGroup("config");
        positionProfile = settings->value("profile").toString();
        settings->endGroup();
    }

    inline QPoint overlapPosition()
    {
        return QPoint(coordWidth - 1, coordHeight - 1);
    }

    void setSizeProfile(int w, int h)
    {
        if (coordHeight == h && coordWidth == w) {
            return;
        }
        coordWidth = w;
        coordHeight = h;

        qDebug() << coordWidth << coordHeight;

        createProfile();
        auto profile = QString("position_%1x%2").arg(w).arg(h);
        if (profile != positionProfile) {
            positionProfile = profile;
        } else {
            loadProfile();
        }
    }

    inline QPoint takeEmptyCoordPos()
    {
        QPoint pos;
        if (freePosList.isEmpty()) {
            pos = overlapPosition();
        } else {
            pos = Coordinate(freePosList.takeFirst()).position();
        }
        return pos;
    }

    inline CoordValue takeEmptyCoordValue()
    {
        return Coordinate(takeEmptyCoordPos()).value();
    }

    void createProfile()
    {
        qDebug() << "createProfile";
        freePosList.clear();
        for (int i = 0; i < coordWidth; ++i) {
            for (int j = 0; j < coordHeight; ++j) {
                qDebug() << "add " << i << j;
                freePosList.push_back(positionValue(i, j));
            }
        }

        idMap.clear();
        for (auto &id : posMap.keys()) {
            idMap.insert(takeEmptyCoordValue(), id);
        }

        posMap.clear();
        for (auto &pos : idMap.keys()) {
            posMap.insert(idMap.value(pos), pos);
        }
    }

    void loadProfile()
    {
        auto settings = Config::instance()->settings();
        settings->beginGroup(positionProfile);
        for (auto &key : settings->allKeys()) {
            auto coords = key.split("_");
            auto x = coords.value(0).toInt();
            auto y = coords.value(1).toInt();
            CoordValue v = positionValue(x, y);
//            idMap.insert(v, settings->value(key).toString());
            posMap.insert(settings->value(key).toString(), v);
        }
        settings->endGroup();
    }

public:
    QMap<int64_t, QString>  idMap;
    QMap<QString, int64_t>  posMap;
    QSettings               idPosSettings;
    QList<int64_t>          freePosList;

    QString                 positionProfile;
    int                     coordWidth;
    int                     coordHeight;
};

GridManager::GridManager(): d(new GridManagerPrivate)
{
    auto config = Config::instance();
    connect(this, &GridManager::setConfig, config, &Config::setItem);
    connect(this, &GridManager::removeConfig, config, &Config::removeItem);
}

GridManager::~GridManager()
{

}

bool GridManager::insert(const QString &id)
{
    auto freePos = emptyCoordPos();
    if (d->posMap.contains(id)) {
        auto pv = d->posMap.value(id);
        freePos = Coordinate(pv).position();
    }
    auto freeValue = Coordinate(freePos).value();

    d->freePosList.removeAll(freeValue);

    qDebug() << "insert" << id << freePos.x() << freePos.y() << d->freePosList.length();
    return insert(freePos.x(), freePos.y(), id);
}

bool GridManager::insert(int x, int y, const QString &id)
{
    auto pv = positionValue(x, y);
    if (d->idMap.contains(pv) && QPoint(x, y) != d->overlapPosition()) {
        qDebug() << "exist" << pv << d->idMap.value(pv);
        return false;
    }

    d->idMap.insert(pv, id);
    d->posMap.insert(id, pv);
    d->freePosList.removeAll(pv);

    emit setConfig(d->positionProfile, positionKey(pv), id);

    return true;
}

bool GridManager::move(const QString &id, int x, int y)
{
    if (!contains(id)) {
        return false;
    }

    auto newpv = positionValue(x, y);
    if (d->idMap.contains(newpv)) {
        qCritical() << x << y << "has item";
        return false;
    }

    auto pv = d->posMap.value(id);
    remove(pv, id);

    qDebug() << "insert" << x << y << id;
    return insert(x, y, id);
}

bool GridManager::move(const QStringList &selecteds, const QString &current, int x, int y)
{
    auto currentPos = Coordinate(d->posMap.value(current)).position();
    auto offset = QPoint(x, y) - currentPos;

    QList<CoordValue> originPosValue;
    QList<CoordValue> destPosValue;
    // check dest is empty;
    auto destPosMap = d->idMap;
    for (auto &id : selecteds) {
        auto pos = Coordinate(d->posMap.value(id)).position();
        originPosValue << d->posMap.value(id);
        destPosMap.remove(d->posMap.value(id));
        auto destPos = pos + offset;
        destPosValue << Coordinate(destPos).value();
    }

    bool confict = false;
    for (auto pv : destPosValue) {
        if (destPosMap.contains(pv)) {
            qDebug() << pv << destPosMap.value(pv);
            confict = true;
            break;
        }
    }

    if (!confict) {
        for (int i = 0; i < selecteds.length(); ++i) {
            remove(selecteds.value(i));
        }
        for (int i = 0; i < selecteds.length(); ++i) {
            auto pos = Coordinate(destPosValue.value(i)).position();
            insert(pos.x(), pos.y(), selecteds.value(i));
        }
        qDebug() << destPosValue;
        return true;
    }

    qDebug() << originPosValue;
    return true;
}

bool GridManager::remove(const QString &id)
{
    auto pv = d->posMap.value(id);
    return remove(pv, id);
}

bool GridManager::remove(int x, int y, const QString &id)
{
    auto pv = positionValue(x, y);
    return remove(pv, id);
}

bool GridManager::remove(int64_t pv, const QString &id)
{
    qDebug() << pv << id;
    d->idMap.remove(pv);
    d->posMap.remove(id);

    if (pv > d->freePosList.last()) {
        d->freePosList.push_back(pv);
    }
    for (int i = 0; i < d->freePosList.length() - 1; ++i) {
        if (pv < d->freePosList.value(i)) {
            d->freePosList.insert(i, pv);
            break;
        }
    }

    d->freePosList.push_back(pv);

    emit removeConfig(d->positionProfile, positionKey(pv));

    return true;
}

bool GridManager::contains(const QString &id)
{
    return d->posMap.contains(id);
}

QPoint GridManager::position(const QString &id)
{
    if (!d->posMap.contains(id)) {
//        qDebug() << id;
    }
    return positionFromValue(d->posMap.value(id));
}

QString GridManager::id(int x, int y)
{
    return d->idMap.value(positionValue(x, y));
}

QPoint GridManager::emptyCoordPos() const
{
    QPoint pos;
    if (d->freePosList.isEmpty()) {
        pos.setX(d->coordWidth - 1);
        pos.setY(d->coordHeight - 1);
    } else {
        pos = Coordinate(d->freePosList.first()).position();
    }
    return pos;
}

QPoint GridManager::takeEmptyCoordPos()
{
    return d->takeEmptyCoordPos();
}

CoordValue GridManager::takeEmptyCoordValue()
{
    return d->takeEmptyCoordValue();
}

CoordValue GridManager::emptyCoordValue() const
{
    return Coordinate(emptyCoordPos()).value();
}

void GridManager::setCoordinateSize(int w, int h)
{
    d->setSizeProfile(w, h);
    emit setConfig("config", "profile", d->positionProfile);
}
