/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef APPTABLEWIDGET_H
#define APPTABLEWIDGET_H

#include "dbusinterface/dbustype.h"

#include <QtCore>
#include <QtGui>
#include <QtWidgets>

class DesktopItem;

class AppTableWidget : public QTableWidget
{
    Q_OBJECT
public:
    explicit AppTableWidget(QWidget *parent = 0);
    AppTableWidget(int rows, int columns, QWidget *parent = 0);
    ~AppTableWidget();

    void init();
    void startDrag(const DesktopItemInfo& info);
    QPixmap getDragPixmap();
    QList<int> getColumnRowByCount(int count);
    void setItemUnChecked();

signals:

public slots:
    void addItems(QList<DesktopItemInfo> itemInfos);
    void handleCellClicked(int row, int column);
    void handleCellPressed(int row, int column);

protected:
    void mouseMoveEvent(QMouseEvent* event);
    void dragEnterEvent(QDragEnterEvent* event);
    void dragMoveEvent(QDragMoveEvent* event);
    void dragLeaveEvent(QDragLeaveEvent* event);
    void mousePressEvent(QMouseEvent* event);

private:
    DesktopItem* m_dragItem;
};

#endif // APPTABLEWIDGET_H
