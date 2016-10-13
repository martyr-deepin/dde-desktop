/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef WIDGETCANVAS_H
#define WIDGETCANVAS_H

#include <QAbstractItemView>
#include <QScopedPointer>

namespace DDE
{
namespace Desktop
{
//1) 48x48 图标的文件名最大宽度为 64;
//2) 64x64 图标的文件名最大宽度为 96;
//3) 96x96 图标的文件名最大宽度为 128;
//4) 128x128 图标的文件名最大宽度为 160;
//5) 256x256 图标的文件名最大宽度为 304;
const QSize CellSize64(64, 64);
const QSize CellSize96(96, 96);
const QSize CellSize128(128, 128);
const QSize CellSize169(160, 160);
const QSize CellSize304(304, 304);
}
}

class WidgetCanvasLogic;
class WidgetCanvas: public QAbstractItemView
{
    Q_OBJECT
public:
    explicit WidgetCanvas(QWidget *parent);
    ~WidgetCanvas();

    // inherint prue virtual function of QAbstractItemView
    virtual QRect visualRect(const QModelIndex &index) const Q_DECL_OVERRIDE;
    virtual void scrollTo(const QModelIndex &index, ScrollHint hint = EnsureVisible) Q_DECL_OVERRIDE;
    virtual QModelIndex indexAt(const QPoint &point) const Q_DECL_OVERRIDE;
    virtual QModelIndex moveCursor(CursorAction cursorAction,
                                   Qt::KeyboardModifiers modifiers) Q_DECL_OVERRIDE;
    virtual int horizontalOffset() const Q_DECL_OVERRIDE;
    virtual int verticalOffset() const Q_DECL_OVERRIDE;
    virtual bool isIndexHidden(const QModelIndex &index) const Q_DECL_OVERRIDE;
    virtual void setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags command) Q_DECL_OVERRIDE;
    virtual QRegion visualRegionForSelection(const QItemSelection &selection) const Q_DECL_OVERRIDE;

    // draw cell
    void paintEvent(QPaintEvent *e) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
public slots:
    void setCellSize(const QSize &sz);
    void setModel(QAbstractItemModel *model) Q_DECL_OVERRIDE;


private:
    Q_DISABLE_COPY(WidgetCanvas)

    QScopedPointer<WidgetCanvasLogic> d;
};

#endif // WIDGETCANVAS_H
