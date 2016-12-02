/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "gridmanager.h"

#include <QPoint>
#include <QRect>
#include <QDebug>

#include "../config/config.h"

#include "apppresenter.h"

inline QString positionKey(QPoint pos)
{
    return QString("%1_%2").arg(pos.x()).arg(pos.y());
}

inline bool qQPointLessThanKey(const QPoint &key1, const QPoint &key2)
{
    return (key1.x() < key2.x()) ||
           ((key1.x() ==  key2.x()) && (key1.y() < key2.y()));
}

template <> inline bool qMapLessThanKey(const QPoint &key1, const QPoint &key2)
{
    return qQPointLessThanKey(key1, key2);
}

class GridManagerPrivate
{
public:
    GridManagerPrivate()
    {
        auto settings = Config::instance()->settings();
        settings->beginGroup(Config::groupGeneral);
        positionProfile = settings->value(Config::keyProfile).toString();
        autoAlign = settings->value(Config::keyAutoAlign).toBool();
        settings->endGroup();

        coordWidth = 0;
        coordHeight = 0;
    }

    inline int gridCount() const
    {
        return  coordHeight * coordWidth;
    }

    inline bool isValid(QPoint pos)
    {
        QRect rect(0, 0, coordWidth, coordHeight);
        return rect.contains(pos);
    }

    inline QPoint overlapPosition() const
    {
        return QPoint(coordWidth - 1, coordHeight - 1);
    }

    inline QPoint gridPosAt(int index) const
    {
        auto x = index / coordHeight;
        auto y = index % coordHeight;
        return QPoint(x, y);
    }

    inline int indexOfGridPos(const QPoint &pos) const
    {
        return pos.x() * coordHeight + pos.y();
    }

    inline QPoint emptyCoordPos() const
    {
        for (int i = 0; i < usedGrids.size(); ++i) {
            if (!usedGrids[i]) {
                return gridPosAt(i);
            }
        }
        return overlapPosition();
    }

    inline QPoint takeEmptyCoordPos()
    {
        for (int i = 0; i < usedGrids.size(); ++i) {
            if (!usedGrids[i]) {
                usedGrids[i] = true;
                return gridPosAt(i);
            }
        }
        return overlapPosition();
    }

    void createProfile()
    {
        usedGrids.clear();
        usedGrids.resize(coordWidth * coordHeight);
        for (int i = 0; i < usedGrids.size(); ++i) {
            usedGrids[i] = false;
        }
        gridItems.clear();
        itemGrids.clear();
    }

    inline bool add(QPoint pos, const QString &itemId)
    {
        if (gridItems.contains(pos)) {
            if (pos != overlapPosition()) {
                qCritical() << "add" << itemId  << "failed."
                            << pos << "grid exist item" << gridItems.value(pos);
                return false;
            } else {
                overlapItemList << gridItems.value(pos);
            }
        }

        gridItems.insert(pos, itemId);
        itemGrids.insert(itemId, pos);
        cacheItemGrids.insert(itemId, pos);
        usedGrids[indexOfGridPos(pos)] = true;

        return true;
    }

    inline bool remove(QPoint pos, const QString &id)
    {
        cacheItemGrids.remove(id);
        if (!itemGrids.contains(id)) {
            return false;
        }

        gridItems.remove(pos);
        itemGrids.remove(id);

        auto usageIndex = indexOfGridPos(pos);
        usedGrids[usageIndex] = false;

        if (!overlapItemList.isEmpty()
                && (pos == overlapPosition())) {
            auto itemId = overlapItemList.takeLast();
            add(pos, itemId);
//            gridItems.insert(pos, itemId);
//            itemGrids.insert(itemId, pos);
//            usedGrids[usageIndex] = true;
        }
        return true;
    }

//    void reAlign()
//    {
//        auto inUsePos = gridItems.keys();

//        qSort(inUsePos.begin(), inUsePos.end(), qQPointLessThanKey);

//        auto allItem = itemGrids;

//        for (int index = 0; index < inUsePos.length(); ++index) {
//            auto gridPos = inUsePos.value(index);
//            auto id = gridItems.value(gridPos);
//            remove(id);
//            auto newPos = gridPosAt(index);
//            add(newPos, id);
//        }

//        for (auto id : allItem.keys()) {
//            add(id);
//        }
//    }

    void loadProfile()
    {
        auto settings = Config::instance()->settings();
        settings->beginGroup(positionProfile);
        for (auto &key : settings->allKeys()) {
            auto coords = key.split("_");
            auto x = coords.value(0).toInt();
            auto y = coords.value(1).toInt();
            cacheItemGrids.insert(settings->value(key).toString(),
                                  QPoint(x, y));
        }
        settings->endGroup();
    }

    inline void loadSizeProfile(int w, int h)
    {
        Q_ASSERT(!(coordHeight == h && coordWidth == w));
        coordWidth = w;
        coordHeight = h;

        createProfile();

        auto profile = QString("Position_%1x%2").arg(w).arg(h);
        if (profile != positionProfile) {
            positionProfile = profile;
        } else {
            loadProfile();
        }

    }

    inline void changeSizeProfileScaed(int w, int h)
    {
        Q_ASSERT(!(coordHeight == h && coordWidth == w));

        qDebug() << "changeSizeProfile" << w << h
                 << coordHeight << coordWidth;
        auto oldGridCount = coordHeight * coordWidth;
        auto newGridCount = w * h;

        auto allItems = itemGrids;
        QVector<int> preferNewIndex;
        QVector<QString> itemIds;

        for (int i = 0; i < usedGrids.length(); ++i) {
            if (usedGrids.value(i)) {
                auto newIndex = i * newGridCount / oldGridCount;
                preferNewIndex.push_back(newIndex);
                itemIds.push_back(gridItems.value(gridPosAt(i)));
            }
        }

        coordHeight = h;
        coordWidth = w;

        createProfile();
        auto profile = QString("Position_%1x%2").arg(w).arg(h);
        if (profile != positionProfile) {
            positionProfile = profile;
        }


        for (int i = 0; i < preferNewIndex.length(); ++i) {
            auto index = preferNewIndex.value(i);
            if (usedGrids.size() > index && !usedGrids.value(index)) {
                add(gridPosAt(index), itemIds.value(i));
            } else {
                auto freePos = takeEmptyCoordPos();
                add(freePos, itemIds.value(i));
            }
            allItems.remove(itemIds.value(i));
        }

        for (auto id : allItems.keys()) {
            auto freePos = takeEmptyCoordPos();
            add(freePos, id);
        }
    }

    inline void changeSizeProfile(int w, int h)
    {
        Q_ASSERT(!(coordHeight == h && coordWidth == w));

        qDebug() << "changeSizeProfile" << w << h
                 << coordHeight << coordWidth;

        auto allItems = itemGrids;
        QVector<int> preferNewIndex;
        QVector<QString> itemIds;

        for (int i = 0; i < usedGrids.length(); ++i) {
            if (usedGrids.value(i)) {
                preferNewIndex.push_back(i);
                itemIds.push_back(gridItems.value(gridPosAt(i)));
            }
        }

        coordHeight = h;
        coordWidth = w;

        createProfile();
        auto profile = QString("Position_%1x%2").arg(w).arg(h);
        if (profile != positionProfile) {
            positionProfile = profile;
        }

        for (int i = 0; i < preferNewIndex.length(); ++i) {
            auto index = preferNewIndex.value(i);
            if (usedGrids.size() > index && !usedGrids.value(index)) {
                add(gridPosAt(index), itemIds.value(i));
            } else {
                auto freePos = takeEmptyCoordPos();
                add(freePos, itemIds.value(i));
            }
            allItems.remove(itemIds.value(i));
        }

        for (auto id : allItems.keys()) {
            auto freePos = takeEmptyCoordPos();
            add(freePos, id);
        }
    }

    void updateGridProfile(int w, int h)
    {
        if (coordHeight == h && coordWidth == w) {
            qDebug() << "mo profile changed";
            return;
        }

        if (0 == coordWidth && 0 == coordHeight)  {
            qDebug() << "loadSizeProfile";
            loadSizeProfile(w, h);
        } else {
            qDebug() << "changeSizeProfile";
            changeSizeProfile(w, h);
        }
    }

public:
    QStringList             overlapItemList;
    QMap<QString, QPoint>   cacheItemGrids;

    QMap<QPoint, QString>   gridItems;
    QMap<QString, QPoint>   itemGrids;
    QVector<bool>           usedGrids;

    QString                 positionProfile;
    int                     coordWidth;
    int                     coordHeight;

    bool                    autoAlign;
};

GridManager::GridManager(): d(new GridManagerPrivate)
{
}

GridManager::~GridManager()
{

}

bool GridManager::add(const QString &id)
{
    if (d->itemGrids.contains(id)) {
        qDebug() << "item exist item" << d->itemGrids.value(id) << id;
        return false;
    }
    QPoint freePos;
    if (d->cacheItemGrids.contains(id) && !d->autoAlign) {
        freePos = d->cacheItemGrids.value(id);
        if (d->gridItems.contains(freePos)) {
            freePos = d->takeEmptyCoordPos();
        }
    } else {
        freePos = d->takeEmptyCoordPos();
    }

    return add(freePos, id);
}

bool GridManager::add(QPoint pos, const QString &id)
{
    auto ret = d->add(pos, id);
    if (ret) {
        emit AppPresenter::instance()->setConfig(d->positionProfile, positionKey(pos), id);
    }
    return ret;
}

bool GridManager::move(const QStringList &selecteds, const QString &current, int x, int y)
{
    auto currentPos = d->itemGrids.value(current);
    auto destPos = QPoint(x, y);
    auto offset = destPos - currentPos;

    QList<QPoint> originPosList;
    QList<QPoint> destPosList;
    // check dest is empty;
    auto destPosMap = d->gridItems;
    auto destUsedGrids = d->usedGrids;
    for (auto &id : selecteds) {
        auto oldPos = d->itemGrids.value(id);
        originPosList << oldPos;
        destPosMap.remove(oldPos);
        destUsedGrids[d->indexOfGridPos(oldPos)] = false;
        auto destPos = oldPos + offset;
        destPosList << destPos;
    }

    bool confict = false;
    for (auto pos : destPosList) {
        if (destPosMap.contains(pos) || !d->isValid(pos)) {
            confict = true;
            break;
        }
    }

    // no need to resize
    if (confict) {
        auto selectedHeadCount = selecteds.indexOf(current);
        // find free grid before destPos
        auto destIndex = d->indexOfGridPos(destPos);

        QList<int> emptyIndexList;

        for (int  i = 0; i < d->gridCount(); ++i) {
            if (false == destUsedGrids.value(i)) {
                emptyIndexList << i;
            }
        }
        auto destGridHeadCount = emptyIndexList.indexOf(destIndex);

        Q_ASSERT(emptyIndexList.length() >= selecteds.length());

        auto startIndex = emptyIndexList.indexOf(destIndex) - selectedHeadCount;
        if (destGridHeadCount < selectedHeadCount) {
            startIndex = 0;
        }
        auto destTailCount = emptyIndexList.length() - destGridHeadCount;
        auto selectedTailCount = selecteds.length() - selectedHeadCount;
        if (destTailCount <= selectedTailCount) {
            startIndex = emptyIndexList.length() - selecteds.length();
        }

        destPosList.clear();

        startIndex = emptyIndexList.value(startIndex);
        for (int i = startIndex; i < d->gridCount(); ++i) {
            if (false == destUsedGrids.value(i)) {
                destPosList << d->gridPosAt(i);
            }
        }

    }

    for (int i = 0; i < selecteds.length(); ++i) {
        remove(selecteds.value(i));
    }
    for (int i = 0; i < selecteds.length(); ++i) {
        add(destPosList.value(i), selecteds.value(i));
    }

    if (d->autoAlign) {
        reAlign();
    }

    return true;
}

bool GridManager::remove(const QString &id)
{
    auto pos = d->itemGrids.value(id);
    return remove(pos, id);
}

bool GridManager::remove(int x, int y, const QString &id)
{
    auto pos = QPoint(x, y);
    return remove(pos, id);
}

bool GridManager::remove(QPoint pos, const QString &id)
{
    auto ret = d->remove(pos, id);
    if (ret) {
        auto newItemId = d->gridItems.value(pos);
        if (newItemId.isEmpty()) {
            emit AppPresenter::instance()->removeConfig(d->positionProfile, positionKey(pos));
        } else {
            emit AppPresenter::instance()->setConfig(d->positionProfile, positionKey(pos), newItemId);
        }
    }
    return ret;
}

bool GridManager::clear()
{
    d->cacheItemGrids.clear();
    d->gridItems.clear();
    d->itemGrids.clear();
    d->createProfile();

    emit AppPresenter::instance()->removeConfig(d->positionProfile, "");

    return true;
}

QString GridManager::firstItemId()
{
    for (int i = 0; i < d->usedGrids.length(); ++i) {
        if (d->usedGrids.value(i)) {
            auto pos = d->gridPosAt(i);
            return  itemId(pos);
        }
    }
    return "";
}

bool GridManager::contains(const QString &id)
{
    return d->itemGrids.contains(id);
}

QPoint GridManager::position(const QString &id)
{
    if (!d->itemGrids.contains(id)) {
        return d->overlapPosition();
    }

    return d->itemGrids.value(id);
}

QString GridManager::itemId(int x, int y)
{
    return d->gridItems.value(QPoint(x, y));
}

QString GridManager::itemId(QPoint pos)
{
    return d->gridItems.value(pos);
}

bool GridManager::isEmpty(int x, int y)
{
    return !d->usedGrids.value(d->indexOfGridPos(QPoint(x, y)));
}

bool GridManager::autoAlign()
{
    return d->autoAlign;
}

void GridManager::toggleAlign()
{
    d->autoAlign = !d->autoAlign;

    if (!d->autoAlign) {
        return;
    }

    reAlign();
}


void GridManager:: reAlign()
{
    auto inUsePos = d->gridItems.keys();

    qSort(inUsePos.begin(), inUsePos.end(), qQPointLessThanKey);

    auto allItem = d->itemGrids;

    qDebug() <<  d->itemGrids.size() <<  d->gridItems.size();

    for (int index = 0; index < inUsePos.length(); ++index) {
        auto gridPos = inUsePos.value(index);
        auto id = d->gridItems.value(gridPos);
        remove(id);
        allItem.remove(id);
        auto newPos = d->gridPosAt(index);
        add(newPos, id);
    }

    qDebug() << allItem.keys();
    for (auto id : allItem.keys()) {
        add(id);
    }
}

void GridManager::updateGridSize(int w, int h)
{
    d->updateGridProfile(w, h);
    if (d->autoAlign) {
        this->reAlign();
    }
    emit AppPresenter::instance()->setConfig(Config::groupGeneral,
            Config::keyProfile,
            d->positionProfile);
}

