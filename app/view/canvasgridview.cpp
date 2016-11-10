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
#include <QPainterPath>
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

#include "../model/dfileselectionmodel.h"
#include "private/canvasviewprivate.h"
#include "canvasviewhelper.h"

DWIDGET_USE_NAMESPACE



CanvasGridView::CanvasGridView(QWidget *parent)
    : QAbstractItemView(parent), d(new CanvasViewPrivate)
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
    setSelectionModel(new DFileSelectionModel(model(), this));
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

QModelIndex CanvasGridView::moveCursorGrid(CursorAction cursorAction, Qt::KeyboardModifiers modifiers)
{
    Q_UNUSED(modifiers);

    auto selections = this->selectionModel();
    auto root = this->rootIndex();
    auto headIndex = model()->index(0, 0, root);
    auto tailIndex = model()->index(model()->rowCount() - 1, 0, root);
    QModelIndex current = currentIndex();
    if (!current.isValid() || !selections->isSelected(current)) {
        return headIndex;
    }

    auto currentRow = current.row();

    auto coord = d->indexCoordinate(currentRow);
    auto newCoord = coord;

    switch (cursorAction) {
    case MoveLeft:
        newCoord = coord.moveLeft();
        break;
    case MoveRight:
        newCoord = coord.moveRight();
        break;
    case MoveUp:
        newCoord = coord.moveUp();
        break;
    case MoveDown:
        newCoord = coord.moveDown();
        break;
    case MovePrevious:
        if (isIndexValid(currentRow - 1)) {
            return model()->index(currentRow - 1, 0, root);
        }
        return headIndex;
    case MoveNext:
        if (isIndexValid(currentRow + 1)) {
            return model()->index(currentRow + 1, 0, root);
        }
        return tailIndex;
    case MoveHome:
    case MovePageUp:
        return headIndex;
    case MoveEnd:
    case MovePageDown:
        return tailIndex;
    }

    if (!d->isVaildCoordinate(newCoord)) {
        return current;
    }

    auto newIndex = d->corrdinateIndex(newCoord);
    if (isIndexValid(newIndex)) {
        return model()->index(newIndex, 0, root);
    }

    return current;
}

QModelIndex CanvasGridView::moveCursor(QAbstractItemView::CursorAction cursorAction, Qt::KeyboardModifiers modifiers)
{
    auto root = this->rootIndex();
    auto current = currentIndex();
    if (!current.isValid()) {
        return model()->index(0, 0, root);
    }

    if (rectForIndex(current).isEmpty()) {
        d->lastCursorIndex = model()->index(0, 0, root);
        return d->lastCursorIndex;
    }

    QModelIndex index;

    switch (cursorAction) {
    case MoveLeft:
        if (DFMGlobal::keyShiftIsPressed()) {
            index = moveCursorGrid(cursorAction, modifiers);
        } else {
            index = moveCursorGrid(cursorAction, modifiers);
        }

        break;
    case MoveRight:
        if (DFMGlobal::keyShiftIsPressed()) {
            index = moveCursorGrid(cursorAction, modifiers);
        } else {
            index = moveCursorGrid(cursorAction, modifiers);
        }

        break;
    default:
        index = moveCursorGrid(cursorAction, modifiers);
        break;
    }

    if (index.isValid()) {
        d->lastCursorIndex = index;

        return index;
    }

    d->lastCursorIndex = current;

    return current;
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
    QItemSelection selection(
        indexAt(rect.topLeft()),
        indexAt(rect.bottomRight()));
    QAbstractItemView::selectionModel()->select(selection, command);
}

QRegion CanvasGridView::visualRegionForSelection(const QItemSelection &selection) const
{
    QRegion region;
    auto selectedList = selection.indexes();
    for (auto &index : selectedList) {
        region = region.united(QRegion(visualRect(index)));
    }
    return region;
}

void CanvasGridView::dragEnterEvent(QDragEnterEvent *event)
{
    for (const DUrl &url : event->mimeData()->urls()) {
        const DAbstractFileInfoPointer &fileInfo = DFileService::instance()->createFileInfo(url);
        if (!fileInfo->isWritable()) {
            event->ignore();
            return;
        }
    }

    if (event->source() == this && !DFMGlobal::keyCtrlIsPressed()) {
        event->setDropAction(Qt::MoveAction);
    } else {
        DAbstractFileInfoPointer info = model()->fileInfo(indexAt(event->pos()));
        if (!info) {
            info = model()->fileInfo(rootIndex());
        }

        if (info && !info->supportedDropActions().testFlag(event->dropAction())) {
            QList<Qt::DropAction> actions;

            actions.reserve(3);
            actions << Qt::CopyAction << Qt::MoveAction << Qt::LinkAction;

            for (Qt::DropAction action : actions) {
                if (event->possibleActions().testFlag(action)
                        && info->supportedDropActions().testFlag(action)) {
                    event->setDropAction(action);
                    break;
                }
            }
        }
    }

    if (event->mimeData()->hasFormat("XdndDirectSave0")) {
        event->setDropAction(Qt::CopyAction);
        event->acceptProposedAction();
        return;
    }

    QAbstractItemView::dragEnterEvent(event);
}

void CanvasGridView::dragMoveEvent(QDragMoveEvent *event)
{
    d->dragMoveHoverIndex = indexAt(event->pos());

    if (d->dragMoveHoverIndex.isValid()) {
        const DAbstractFileInfoPointer &fileInfo = model()->fileInfo(d->dragMoveHoverIndex);

        if (fileInfo) {
            if (!fileInfo->isDir()) {
                d->dragMoveHoverIndex = QModelIndex();
            } else if (!fileInfo->supportedDropActions().testFlag(event->dropAction())) {
                d->dragMoveHoverIndex = QModelIndex();

                update();
                return event->ignore();
            }
        }
    }

    update();

    if (dragDropMode() == InternalMove
            && (event->source() != this || !(event->possibleActions() & Qt::MoveAction))) {
        QAbstractItemView::dragMoveEvent(event);
    }
}

void CanvasGridView::dragLeaveEvent(QDragLeaveEvent *event)
{
    d->dragMoveHoverIndex = QModelIndex();
    QAbstractItemView::dragLeaveEvent(event);
}

void CanvasGridView::dropEvent(QDropEvent *event)
{
    d->dragMoveHoverIndex = QModelIndex();

    if (event->source() == this && !DFMGlobal::keyCtrlIsPressed()) {
        event->setDropAction(Qt::MoveAction);
    } else {
        DAbstractFileInfoPointer info = model()->fileInfo(indexAt(event->pos()));
        if (!info) {
            info = model()->fileInfo(rootIndex());
        }

        if (info && !info->supportedDropActions().testFlag(event->dropAction())) {
            QList<Qt::DropAction> actions;

            actions.reserve(3);
            actions << Qt::CopyAction << Qt::MoveAction << Qt::LinkAction;

            for (Qt::DropAction action : actions) {
                if (event->possibleActions().testFlag(action)
                        && info->supportedDropActions().testFlag(action)) {
                    event->setDropAction(action);
                    break;
                }
            }
        }
    }

    if (event->mimeData()->property("IsDirectSaveMode").toBool()) {
        event->setDropAction(Qt::CopyAction);

        const QModelIndex &index = indexAt(event->pos());
        const DAbstractFileInfoPointer &fileInfo = model()->fileInfo(index.isValid() ? index : rootIndex());

        if (fileInfo && fileInfo->fileUrl().isLocalFile()) {
            if (fileInfo->isDir()) {
                const_cast<QMimeData *>(event->mimeData())->setProperty("DirectSaveUrl", fileInfo->fileUrl());
            } else {
                const_cast<QMimeData *>(event->mimeData())->setProperty("DirectSaveUrl", fileInfo->parentUrl());
            }
        }

        event->accept(); // yeah! we've done with XDS so stop Qt from further event propagation.
    } else {
        QModelIndex index = indexAt(event->pos());

        if (!index.isValid()) {
            index = rootIndex();
        }

        if (!index.isValid()) {
            return;
        }

        if (static_cast<DFileSelectionModel *>(selectionModel())->isSelected(index)) {
            return;
        }

        if (model()->supportedDropActions() & event->dropAction() && model()->flags(index) & Qt::ItemIsDropEnabled) {
            const Qt::DropAction action = dragDropMode() == InternalMove ? Qt::MoveAction : event->dropAction();
            if (model()->dropMimeData(event->mimeData(), action, index.row(), index.column(), index)) {
                if (action != event->dropAction()) {
                    event->setDropAction(action);
                    event->accept();
                } else {
                    event->acceptProposedAction();
                }
            }
        }

        stopAutoScroll();
        setState(NoState);
        viewport()->update();
    }
}

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

QRect CanvasGridView::rectForIndex(const QModelIndex &index) const
{
    return visualRect(index);
}

DUrl CanvasGridView::currentUrl() const
{
    return model()->getUrlByIndex(rootIndex());
}

DFileSystemModel *CanvasGridView::model() const
{
    return qobject_cast<DFileSystemModel *>(QAbstractItemView::model());
}

DFileSelectionModel *CanvasGridView::selectionModel() const
{
    return static_cast<DFileSelectionModel *>(QAbstractItemView::selectionModel());
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

bool CanvasGridView::isIndexValid(int index)
{
    return model()->index(index, 0, this->rootIndex()).isValid();
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
