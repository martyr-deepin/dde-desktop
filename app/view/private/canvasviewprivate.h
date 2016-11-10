/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#pragma once

#include <QtGlobal>
#include <QModelIndex>
#include <QSize>
#include <QPoint>
#include <QMargins>

class CanvasViewHelper;
class Coordinate
{
public:
    int x;
    int y;

    Coordinate(int _x, int _y): x(_x), y(_y) {}

    Coordinate moveLeft(int offset = 1) const
    {
        return Coordinate(x - offset, y);
    }

    Coordinate moveRight(int offset = 1) const
    {
        return Coordinate(x + offset, y);
    }

    Coordinate moveUp(int offset = 1) const
    {
        return Coordinate(x, y - offset);
    }

    Coordinate moveDown(int offset = 1) const
    {
        return Coordinate(x, y + offset);
    }

};

class CanvasViewPrivate
{
public:
    CanvasViewPrivate();

    void updateSize(const QSize &sz)
    {
        Q_ASSERT(cellHeight != 0);
        Q_ASSERT(cellWidth != 0);
        rowCount = sz.height() / cellHeight;
        auto verticalMargin = (sz.height() % cellHeight) / 2;
        colCount = sz.width() / cellWidth;
        auto horizontalMargin = (sz.width() % cellWidth) / 2;
        viewMargins = QMargins(horizontalMargin, verticalMargin, horizontalMargin, verticalMargin);
    }

    void updateCellSize(const QSize &sz)
    {
        cellWidth = sz.width();
        cellHeight = sz.height();
    }

    Coordinate indexCoordinate(int index)
    {
        return Coordinate(index / rowCount, index % rowCount);
    }

    int corrdinateIndex(Coordinate coord)
    {
        return coord.x * rowCount + coord.y;
    }

    bool isVaildCoordinate(Coordinate coord)
    {
        return (coord.x >= 0 && coord.x < colCount)
               && (coord.y >= 0 && coord.y < rowCount);
    }
public:
    QMargins viewMargins;
    QMargins cellMargins;
    int rowCount;
    int colCount;
    int cellWidth;
    int cellHeight;

    QModelIndex         dragMoveHoverIndex;
    QModelIndex         lastCursorIndex;

    CanvasViewHelper    *fileViewHelper = nullptr;
};
