/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef CANVASGRIDVIEW_H
#define CANVASGRIDVIEW_H

#include <QAbstractItemView>
#include <QScopedPointer>

class DUrl;
class DStyledItemDelegate;
class DFileSystemModel;
class WidgetCanvasLogic;
class CanvasGridView: public QAbstractItemView
{
    Q_OBJECT
public:
    explicit CanvasGridView(QWidget *parent);
    ~CanvasGridView();

    // inherint prue virtual function of QAbstractItemView
    virtual QRect visualRect(const QModelIndex &index) const Q_DECL_OVERRIDE;
    virtual QModelIndex indexAt(const QPoint &point) const Q_DECL_OVERRIDE;
    virtual void scrollTo(const QModelIndex &index,
                          ScrollHint hint = EnsureVisible) Q_DECL_OVERRIDE;
    virtual QModelIndex moveCursor(CursorAction cursorAction,
                                   Qt::KeyboardModifiers modifiers) Q_DECL_OVERRIDE;
    virtual int horizontalOffset() const Q_DECL_OVERRIDE;
    virtual int verticalOffset() const Q_DECL_OVERRIDE;
    virtual bool isIndexHidden(const QModelIndex &index) const Q_DECL_OVERRIDE;
    virtual void setSelection(const QRect &rect,
                              QItemSelectionModel::SelectionFlags command) Q_DECL_OVERRIDE;
    virtual QRegion visualRegionForSelection(const QItemSelection &selection) const Q_DECL_OVERRIDE;

    void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;

    DUrl currentUrl() const;
    bool setCurrentUrl(const DUrl &url);
    bool setRootUrl(const DUrl &url);
    // draw cell

    DFileSystemModel *model() const;
    DStyledItemDelegate *itemDelegate() const;
    void setItemDelegate(DStyledItemDelegate *delegate);
private:
    Q_DISABLE_COPY(CanvasGridView)

    QScopedPointer<WidgetCanvasLogic> d;
};


#endif // CANVASGRIDVIEW_H
