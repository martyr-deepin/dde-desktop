/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "canvasgridview.h"

#include <QScrollBar>
#include <QPainter>
#include <QResizeEvent>
#include <QDebug>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QUrlQuery>
#include <QActionGroup>
#include <QContextMenuEvent>
#include <QHeaderView>
#include <QMimeData>

#include <dthememanager.h>
#include <dscrollbar.h>
#include <dslider.h>
#include <anchors.h>
#include <DAction>

#include <durl.h>
#include <dfmglobal.h>
#include <dfilesystemmodel.h>
#include <diconitemdelegate.h>
#include <dlistitemdelegate.h>
#include <dfileviewhelper.h>
#include <dfileservices.h>
#include <dfileinfo.h>
#include <dfilemenu.h>
#include <dfilemenumanager.h>

#include "canvasviewhelper.h"
#include "../model/dfileselectionmodel.h"

template<typename T>
class Singleton
{
public:
    static T *instance()
    {
        static T instance;
        return &instance;
    }

private:
    Singleton();
    ~Singleton();
    Singleton(const Singleton &);
    Singleton &operator = (const Singleton &);
};

#define fileManagerApp Singleton<FileManagerApp>::instance()
#define fileService DFileService::instance()
#define fileMenuManger  Singleton<DFileMenuManager>::instance()

DWIDGET_USE_NAMESPACE

class Coordinate
{
public:
    int x;
    int y;

    Coordinate(int _x, int _y): x(_x), y(_y) {}
};

class WidgetCanvasLogic
{
public:
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

    Coordinate posCoordinate(QPoint pos)
    {
        return Coordinate(0, 0);
    }

public:
    CanvasViewHelper *fileViewHelper;
    QMargins viewMargins;
    QMargins cellMargins;
    int rowCount;   // x
    int colCount;   // y
    int cellWidth;
    int cellHeight;
};

CanvasGridView::CanvasGridView(QWidget *parent)
    : QAbstractItemView(parent), d(new WidgetCanvasLogic)
{
    D_THEME_INIT_WIDGET(WidgetCanvas);

    setAttribute(Qt::WA_TranslucentBackground);
    viewport()->setAttribute(Qt::WA_TranslucentBackground);

    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setAcceptDrops(true);
    setDragDropMode(QAbstractItemView::DragDrop);

    d->cellMargins = QMargins(5, 5, 5, 5);
    d->fileViewHelper = new CanvasViewHelper(this);
    setModel(new DFileSystemModel(d->fileViewHelper));
//    setSelectionModel(new DFileSelectionModel(model(), this));
    setItemDelegate(new DIconItemDelegate(d->fileViewHelper));
}

CanvasGridView::~CanvasGridView()
{

}

QRect CanvasGridView::visualRect(const QModelIndex &index) const
{
    auto  coord = d->indexCoordinate(index.row());
    auto x = coord.x * d->cellWidth + d->viewMargins.left();
    auto y = coord.y * d->cellHeight + d->viewMargins.top();
    return QRect(x, y, d->cellWidth, d->cellHeight).marginsRemoved(d->cellMargins);
}

QModelIndex CanvasGridView::indexAt(const QPoint &point) const
{
    auto row = (point.x() - d->viewMargins.left()) / d->cellWidth;
    auto col = (point.y() - d->viewMargins.top()) / d->cellHeight;
    auto rowIndex = d->corrdinateIndex(Coordinate(row, col));
    auto viewRect = visualRect(model()->index(rowIndex, 0));

    if (viewRect.contains(point)) {
        return model()->index(rowIndex, 0);
    }
    return QModelIndex();
}
#include <QListView>

void CanvasGridView::scrollTo(const QModelIndex &index, QAbstractItemView::ScrollHint hint)
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

QModelIndex CanvasGridView::moveCursor(QAbstractItemView::CursorAction cursorAction, Qt::KeyboardModifiers modifiers)
{
    qDebug() << cursorAction << modifiers;
    return QModelIndex();
}

int CanvasGridView::horizontalOffset() const
{
    return horizontalScrollBar()->value();
}

int CanvasGridView::verticalOffset() const
{
    return verticalScrollBar()->value();
}

bool CanvasGridView::isIndexHidden(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return false;
}

void CanvasGridView::setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags command)
{
    qDebug() << rect << command;
    QItemSelection selection(
        indexAt(rect.topLeft()),
        indexAt(rect.bottomRight()));
    selectionModel()->select(selection, command);
}

QRegion CanvasGridView::visualRegionForSelection(const QItemSelection &selection) const
{
    QRegion region;
    auto selectedList = selection.indexes();
    for (auto &index : selectedList) {
        region = region.united(QRegion(visualRect(index)));
    }
    qDebug() << selection << region;
    return region;
}
#include <QPainterPath>
void CanvasGridView::paintEvent(QPaintEvent *)
{
    QPainter painter(viewport());

    auto option = viewOptions();
    const QModelIndex current = currentIndex();
//    const QModelIndex hover = d->hover;
    const QAbstractItemModel *itemModel = this->model();
//    const QItemSelectionModel *selections = ;
    const bool focus = (hasFocus() || viewport()->hasFocus()) && current.isValid();
//    const bool alternate = d->alternatingColors;
    const QStyle::State state = option.state;
    const QAbstractItemView::State viewState = this->state();
    const bool enabled = (state & QStyle::State_Enabled) != 0;

    bool alternateBase = false;
    int previousRow = -2; // trigger the alternateBase adjustment on first pass

    int maxSize = viewport()->size().width();

    for (int i = 0; i < this->model()->rowCount(); ++i) {
        auto index = this->model()->index(i, 0) ;
        Q_ASSERT(index.isValid());
        option.rect = visualRect(index);
        QPainterPath path;
        path.addRect(option.rect);
        painter.strokePath(path, QColor(255, 0, 0));
        option.rect.marginsRemoved(QMargins(5, 5, 5, 5));
        option.state = state;
//        if (selections && selections->isSelected(*it)) {
//            option.state |= QStyle::State_Selected;
//        }
        if (enabled) {
            QPalette::ColorGroup cg;
            if ((itemModel->flags(index) & Qt::ItemIsEnabled) == 0) {
                option.state &= ~QStyle::State_Enabled;
                cg = QPalette::Disabled;
            } else {
                cg = QPalette::Normal;
            }
            option.palette.setCurrentColorGroup(cg);
        }
        if (focus && current == index) {
            option.state |= QStyle::State_HasFocus;
            if (viewState == EditingState) {
                option.state |= QStyle::State_Editing;
            }
        }
//        if (index == hover) {
//            option.state |= QStyle::State_MouseOver;
//        } else {
//            option.state &= ~QStyle::State_MouseOver;
//        }

        this->itemDelegate()->paint(&painter, option, index);
    }

    painter.setBrush(QColor(255, 0, 0, 0));
    if (model()) {
        for (int i = 0; i < d->colCount * d->rowCount; ++i) {
            auto  coord = d->indexCoordinate(i);
            auto x = coord.x * d->cellWidth + d->viewMargins.left();
            auto y = coord.y * d->cellHeight + d->viewMargins.top();
            auto rect =  QRect(x, y, d->cellWidth, d->cellHeight);

            int rowMode = coord.x % 2;
            int colMode = coord.y % 2;
            auto color = (colMode == rowMode) ? QColor(0, 0, 255, 25) : QColor(255, 0, 0, 25);
            painter.fillRect(rect, color);
        }
    }

}

void CanvasGridView::resizeEvent(QResizeEvent *event)
{
    auto itemSize = itemDelegate()->sizeHint(QStyleOptionViewItem(), QModelIndex());
    auto itemRect = QRect(0, 0, itemSize.width(), itemSize.height());
    auto cellRect = itemRect.marginsAdded(d->cellMargins);
    auto cellSize = cellRect.size();
    d->cellWidth = cellSize.width();
    d->cellHeight = cellSize.height();

    d->updateSize(event->size());
    setContentsMargins(d->viewMargins);
    // todo restore
}

DUrl CanvasGridView::currentUrl() const
{
    return model()->getUrlByIndex(rootIndex());
}

DFileSystemModel *CanvasGridView::model() const
{

    return qobject_cast<DFileSystemModel *>(QAbstractItemView::model());
}

DStyledItemDelegate *CanvasGridView::itemDelegate() const
{

    return qobject_cast<DStyledItemDelegate *>(QAbstractItemView::itemDelegate());
}

void CanvasGridView::setItemDelegate(DStyledItemDelegate *delegate)
{
    QAbstractItemDelegate *dg = QAbstractItemView::itemDelegate();

    if (dg) {
        dg->deleteLater();
    }

    QAbstractItemView::setItemDelegate(delegate);

//    connect(delegate, &DStyledItemDelegate::commitData, this, &CanvasGridView::handleCommitData);
}

bool CanvasGridView::setCurrentUrl(const DUrl &url)
{
    DUrl fileUrl = url;
    const DAbstractFileInfoPointer &info = DFileService::instance()->createFileInfo(fileUrl);
    if (!info) {
        qDebug() << "This scheme isn't support";
        return false;
    }

    qDebug() << "cd: current url:" << currentUrl() << "to url:" << fileUrl;

    const DUrl &checkUrl = currentUrl();

    if (checkUrl == fileUrl) {
        return false;
    }

    QModelIndex index = model()->setRootUrl(fileUrl);
    setRootIndex(index);

    if (!model()->canFetchMore(index)) {
        qDebug() << "updateContentLabel()";
    }
    // TODO: set sort

    if (focusWidget() && focusWidget()->window() == window() && fileUrl.isLocalFile()) {
        QDir::setCurrent(fileUrl.toLocalFile());
    }

    return true;
}


bool CanvasGridView::setRootUrl(const DUrl &url)
{
    if (url.isEmpty()) {
        return false;
    }

    itemDelegate()->hideAllIIndexWidget();

    clearSelection();

    if (!url.isSearchFile()) {
        setFocus();
    }

    return setCurrentUrl(url);
}
