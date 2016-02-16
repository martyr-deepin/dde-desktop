/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef GRIDITEM_H
#define GRIDITEM_H

#include <QtCore>

class GridItem : public QObject
{
    Q_OBJECT
public:
    explicit GridItem(int row, int column, QRect rect, QObject *parent = 0);
    ~GridItem();

    int getColumn();
    void setColumn(const int column);
    int getRow();
    void setRow(const int row);
    QRect getRect();
    void setRect(const QRect rect);
    QPoint getPos();
    QString key();

    bool hasDesktopItem();
    bool isMultiDesktopItemsIn();

signals:

public slots:
    void setDesktopItem(bool flag);
    void setMultiDesktopItemsIn(bool flag);


private:
    int m_column;
    int m_row;
    QRect m_rect;
    QPoint m_pos;
    bool m_isDesktopItemIn;
    bool m_isMultiDesktopItemsIn=false;
};

#endif // GRIDITEM_H
