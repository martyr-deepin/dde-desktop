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

#include <dlistview.h>
#include <QDir>

class DFileSystemModel;
QT_BEGIN_NAMESPACE
class QHeaderView;
class QTimer;
QT_END_NAMESPACE

DWIDGET_BEGIN_NAMESPACE
class DAction;
DWIDGET_END_NAMESPACE

DWIDGET_USE_NAMESPACE

class DUrl;
class FileController;
class DFileMenuManager;
class DFileSystemModel;
class DStyledItemDelegate;
class DAbstractFileInfo;
class DStatusBar;
class CanvasViewHelper;
class CanvasViewPrivate;

class CanvasView : public DListView
{
    Q_OBJECT
public:
    enum ViewMode {
        IconMode = 0x01,
    };

    Q_DECLARE_FLAGS(ViewModes, ViewMode)

    explicit CanvasView(QWidget *parent = 0);
    ~CanvasView();

    // inherint prue virtual function of QAbstractItemView
    virtual QRect visualRect(const QModelIndex &index) const Q_DECL_OVERRIDE;
    virtual QModelIndex indexAt(const QPoint &point) const Q_DECL_OVERRIDE;
    virtual QModelIndex moveCursor(CursorAction cursorAction,
                                   Qt::KeyboardModifiers modifiers) Q_DECL_OVERRIDE;
    virtual int horizontalOffset() const Q_DECL_OVERRIDE;
    virtual int verticalOffset() const Q_DECL_OVERRIDE;
    virtual void setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags command) Q_DECL_OVERRIDE;


//    int horizontalOffset() const Q_DECL_OVERRIDE;
//    QModelIndex indexAt(const QPoint &point) const Q_DECL_OVERRIDE;
//    QRect visualRect(const QModelIndex &index) const Q_DECL_OVERRIDE;
//    void setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags flags) Q_DECL_OVERRIDE;
//    QModelIndex moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers) Q_DECL_OVERRIDE;

    void initDelegate();
    void initUI();
    void initModel();
    void initConnects();


    DFileSystemModel *model() const;

    DStyledItemDelegate *itemDelegate() const;
    void setItemDelegate(DStyledItemDelegate *delegate);

    CanvasViewHelper *fileViewHelper() const;

    DUrl currentUrl() const;
    QList<DUrl> selectedUrls() const;

    int rowCount() const;

    /// icon view row index count
    int itemCountForRow() const;
    inline int indexOfRow(const QModelIndex &index) const
    { return index.row() / itemCountForRow();}

    using DListView::edit;

    void setIconSize(const QSize &size);

    int getSortRoles() const;

    bool isSelected(const QModelIndex &index) const;
    int selectedIndexCount() const;
    QModelIndexList selectedIndexes() const Q_DECL_OVERRIDE;


    typedef QPair<int, int> RandeIndex;
    typedef QList<RandeIndex> RandeIndexList;
    RandeIndexList visibleIndexes(QRect rect) const;

    QSize itemSizeHint() const;

    using DListView::viewportMargins;
    using DListView::updateGeometries;

    bool isDropTarget(const QModelIndex &index) const;

public slots:
    bool cd(const DUrl &url);
    bool cdUp();
    bool edit(const QModelIndex &index, EditTrigger trigger, QEvent *event) Q_DECL_OVERRIDE;
    void select(const QList<DUrl> &list);

    void sortByRole(int role);

    QStringList nameFilters() const;
    void setNameFilters(const QStringList &nameFilters);
    QDir::Filters filters() const;
    void setFilters(QDir::Filters filters);

    void clearSelection();

    void handleDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles = QVector<int>());

    void setSelectionRectVisible(bool visible);
    bool isSelectionRectVisible() const;
    bool canShowSelectionRect() const;


signals:
    void currentUrlChanged(const DUrl &url);

private slots:
    bool setCurrentUrl(const DUrl &url);
    void sortByActionTriggered(QAction *action);
    void openWithActionTriggered(QAction *action);
    void onRowCountChanged();
    void handleCommitData(QWidget *editor);

protected:
    void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
    void showEvent(QShowEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void focusInEvent(QFocusEvent *event) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
    void contextMenuEvent(QContextMenuEvent *event) Q_DECL_OVERRIDE;
    void dragEnterEvent(QDragEnterEvent *event) Q_DECL_OVERRIDE;
    void dragMoveEvent(QDragMoveEvent *event) Q_DECL_OVERRIDE;
    void dragLeaveEvent(QDragLeaveEvent *event) Q_DECL_OVERRIDE;
    void dropEvent(QDropEvent *event) Q_DECL_OVERRIDE;
    void rowsAboutToBeRemoved(const QModelIndex &parent, int start, int end) Q_DECL_OVERRIDE;
    void rowsInserted(const QModelIndex &parent, int start, int end) Q_DECL_OVERRIDE;

private:
    void increaseIcon();
    void decreaseIcon();
    void openIndex(const QModelIndex &index);
    void keyboardSearch(const QString &search) Q_DECL_OVERRIDE;
    void updateHorizontalOffset();
    void switchViewMode(ViewMode mode);
    void showEmptyAreaMenu();
    void showNormalMenu(const QModelIndex &index);
    void updateListHeaderViewProperty();
    void updateExtendHeaderViewProperty();
    void updateColumnWidth();
    void popupHeaderViewContextMenu(const QPoint &pos);
    void onModelStateChanged(int state);
    void updateSelectionRect();

    using DListView::setOrientation;

    void preproccessDropEvent(QDropEvent *event) const;

    QScopedPointer<CanvasViewPrivate> d_ptr;

    Q_DECLARE_PRIVATE_D(qGetPtrHelper(d_ptr), CanvasView)
};

#endif // WIDGETCANVAS_H
