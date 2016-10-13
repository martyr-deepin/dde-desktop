/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "widgetcanvas.h"

#include <QScrollBar>
#include <QPainter>
#include <QResizeEvent>
#include <QDebug>

#include "dthememanager.h"

#include "../presenter/widgetpresenter.h"

DWIDGET_USE_NAMESPACE

class WidgetCanvasLogic
{
public:
    void updateSize(const QSize &sz)
    {
        qDebug() << sz << cellHeight << cellWidth;
        Q_ASSERT(cellHeight != 0);
        Q_ASSERT(cellWidth != 0);
        rowCount = sz.height() / cellWidth;
        auto verticalMargin = (sz.height() % cellWidth) / 2;
        colCount = sz.width() / cellHeight;
        auto horizontalMargin = (sz.width() % cellHeight) / 2;
        margins = QMargins(horizontalMargin, verticalMargin, horizontalMargin, verticalMargin);
    }

    void updateCellSize(const QSize &sz)
    {
        cellWidth = sz.width();
        cellHeight = sz.height();
    }

public:
    QMargins margins;
    int rowCount;   // x
    int colCount;   // y
    int cellWidth;
    int cellHeight;
};

WidgetCanvas::WidgetCanvas(QWidget *parent)
    : QAbstractItemView(parent), d(new WidgetCanvasLogic)
{
    D_THEME_INIT_WIDGET(WidgetCanvas);

    setSelectionMode(QAbstractItemView::MultiSelection);
    setAcceptDrops(true);
    setDragEnabled(true);
    setDragDropMode(QAbstractItemView::DragDrop);
    viewport()->setAcceptDrops(true);
    setDropIndicatorShown(false);
}

WidgetCanvas::~WidgetCanvas()
{

}

QRect WidgetCanvas::visualRect(const QModelIndex &index) const
{
    auto x = index.row() * d->cellWidth + d->margins.left();
    auto y = index.column() * d->cellHeight + d->margins.top();

//    qDebug() << index.column() << index.row() << x << y;
    return QRect(x, y, d->cellWidth, d->cellHeight);
}

void WidgetCanvas::scrollTo(const QModelIndex &index, QAbstractItemView::ScrollHint hint)
{
    Q_UNUSED(hint);
    QRect area = viewport()->rect();
    QRect rect = visualRect(index);

    if (rect.left() < area.left()) {
        horizontalScrollBar()->setValue(
            horizontalScrollBar()->value() + rect.left() - area.left());
    } else if (rect.right() > area.right()) {
        horizontalScrollBar()->setValue(
            horizontalScrollBar()->value() + qMin(
                rect.right() - area.right(), rect.left() - area.left()));
    }

    if (rect.top() < area.top()) {
        verticalScrollBar()->setValue(
            verticalScrollBar()->value() + rect.top() - area.top());
    } else if (rect.bottom() > area.bottom()) {
        verticalScrollBar()->setValue(
            verticalScrollBar()->value() + qMin(
                rect.bottom() - area.bottom(), rect.top() - area.top()));
    }

    update();
}

QModelIndex WidgetCanvas::indexAt(const QPoint &point) const
{
//    qDebug() << point << model()->index(point.x() / 100, point.y() / 100);
    auto row = (point.x() - d->margins.left()) / d->cellWidth;
    auto col = (point.y() - d->margins.top()) / d->cellHeight;
    return model()->index(row, col);
}

QModelIndex WidgetCanvas::moveCursor(QAbstractItemView::CursorAction cursorAction, Qt::KeyboardModifiers modifiers)
{
    qDebug() << cursorAction << modifiers;
    return QModelIndex();
}

int WidgetCanvas::horizontalOffset() const
{
    return horizontalScrollBar()->value();
}

int WidgetCanvas::verticalOffset() const
{
    return verticalScrollBar()->value();
}

bool WidgetCanvas::isIndexHidden(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return false;
}

#include <QLabel>
void WidgetCanvas::setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags command)
{
    qDebug() << rect << command;
    auto label = qobject_cast<QLabel *>(this->indexWidget(model()->index(0, 0, rootIndex())));
    if (label) { label->setText("see"); }
    QItemSelection selection(
        indexAt(rect.topLeft()),
        indexAt(rect.bottomRight()));
    selectionModel()->select(selection, command);
}

QRegion WidgetCanvas::visualRegionForSelection(const QItemSelection &selection) const
{
    qDebug() << selection;
    return QRegion(0, 0, 100, 100);
}

void WidgetCanvas::paintEvent(QPaintEvent *e)
{
    QPainter painter(viewport());

    QRect box = geometry().marginsRemoved(d->margins);

    painter.drawRect(box);

    if (model()) {
//    qDebug() << d->colCount << d->rowCount;
        for (int i = 0; i < d->colCount; ++i) {
            for (int j = 0; j < d->rowCount; ++j) {
                painter.drawRect(visualRect(model()->index(i, j, rootIndex())));
            }
        }
    }
//    painter.drawImage(0, 0, QImage("/usr/share/icons/hicolor/512x512/apps/owncloud.png"));

}

void WidgetCanvas::resizeEvent(QResizeEvent *event)
{
    d->updateSize(event->size());
    qDebug() << "resizeEvent" << d->margins;
    setContentsMargins(d->margins);
    // todo restore
}

void WidgetCanvas::setCellSize(const QSize &sz)
{
    d->updateCellSize(sz);
}

void WidgetCanvas::setModel(QAbstractItemModel *model)
{
    qDebug() << "Reset Model";
    connect(model, &QAbstractItemModel::dataChanged, this, [ = ](const QModelIndex & topLeft, const QModelIndex & bottomRight, const QVector<int> &roles = QVector<int>()) {
        qDebug() << "data change" << topLeft << bottomRight << roles;
        auto w = (topLeft.data().value<QWidget*>());
        qDebug() << w;
        this->setIndexWidget(topLeft, w);
    });
    QAbstractItemView::setModel(model);
}
