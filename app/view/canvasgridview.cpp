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
#include <QProcess>

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


#include "../presenter/positionmanager.h"

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

    connect(this->model(), &QAbstractItemModel::rowsInserted,
    this, [ = ](const QModelIndex & parent, int first, int last) {
        qDebug() << parent << first << last;
        for (int i = first; i <= last; ++i) {
            auto index = model()->index(i, 0, parent);
            auto localFile = model()->getUrlByIndex(index).toLocalFile();
            GridManager::instance()->insert(localFile);
        }
        this->repaint();
    });
    connect(this->model(), &QAbstractItemModel::rowsAboutToBeRemoved,
    this, [ = ](const QModelIndex & parent, int first, int last) {
        qDebug() << parent << first << last;
        qDebug() << model()->getUrlByIndex(parent).toLocalFile();
        for (int i = first; i <= last; ++i) {
            auto index = model()->index(i, 0, parent);
            auto localFile = model()->getUrlByIndex(index).toLocalFile();
            qDebug() << "remove" << localFile;
            GridManager::instance()->remove(localFile);
        }
    });
    connect(this->model(), &QAbstractItemModel::rowsRemoved,
    this, [ = ](const QModelIndex & parent, int first, int last) {
        this->repaint();
    });
    connect(this->model(), &QAbstractItemModel::dataChanged,
    this, [ = ](const QModelIndex & topLeft, const QModelIndex & bottomRight, const QVector<int> &roles) {

        this->repaint();
        qDebug() << topLeft << bottomRight << roles;
    });

    connect(this, &CanvasGridView::doubleClicked,
    this, [this](const QModelIndex & index) {
        DFMEvent event;
        DUrl url = model()->getUrlByIndex(index);

        DUrlList urls;
        urls << url;
        event << url;
        event << urls;
        event << DFMEvent::FileView;
        event << this->winId();

        qDebug() << index << url <<
                 url.isValid();

        QFileInfo info(url.toLocalFile());
        if (info.isDir()) {
            QProcess::startDetached("gvfs-open", QStringList() << url.toLocalFile());
        } else {
            DFileService::instance()->openFile(url);
        }
    }, Qt::QueuedConnection);
}

CanvasGridView::~CanvasGridView()
{

}

QRect CanvasGridView::visualRect(const QModelIndex &index) const
{
    auto url = model()->getUrlByIndex(index);
    auto pos = GridManager::instance()->position(url.toLocalFile());

    auto x = pos.x() * d->cellWidth + d->viewMargins.left();
    auto y = pos.y() * d->cellHeight + d->viewMargins.top();

    return QRect(x, y, d->cellWidth, d->cellHeight).marginsRemoved(d->cellMargins);
}

QModelIndex CanvasGridView::indexAt(const QPoint &point) const
{
    auto row = (point.x() - d->viewMargins.left()) / d->cellWidth;
    auto col = (point.y() - d->viewMargins.top()) / d->cellHeight;
    auto localFile =  GridManager::instance()->id(row, col);
    auto rowIndex = model()->index(DUrl::fromLocalFile(localFile));
    auto viewRect = visualRect(rowIndex);

    if (viewRect.contains(point)) {
        return rowIndex;
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

void CanvasGridView::mouseMoveEvent(QMouseEvent *event)
{

    QAbstractItemView::mouseMoveEvent(event);
}

void CanvasGridView::mousePressEvent(QMouseEvent *event)
{
    auto selectIndex = indexAt(event->pos());
    d->lastCursorIndex = selectIndex;

    QAbstractItemView::mousePressEvent(event);
}

void CanvasGridView::mouseReleaseEvent(QMouseEvent *event)
{
    QAbstractItemView::mouseReleaseEvent(event);
}

void CanvasGridView::mouseDoubleClickEvent(QMouseEvent *event)
{
    QModelIndex index = indexAt(event->pos());
    QPersistentModelIndex persistent = index;
    emit doubleClicked(persistent);
    if ((event->button() == Qt::LeftButton) && !edit(persistent, DoubleClicked, event)
            && !style()->styleHint(QStyle::SH_ItemView_ActivateItemOnSingleClick, 0, this)) {
        emit activated(persistent);
    }
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
        if (!index.isValid() || selectionModel()->isSelected(index)) {
            // move pos
            QStringList selectLocalFiles;
            auto selects = selectionModel()->selectedIndexes();
            for (auto index : selects) {
                auto info = model()->fileInfo(index);
                selectLocalFiles << info->fileUrl().toLocalFile();
            }
            auto point = event->pos();
            auto row = (point.x() - d->viewMargins.left()) / d->cellWidth;
            auto col = (point.y() - d->viewMargins.top()) / d->cellHeight;
            auto current = model()->fileInfo(d->lastCursorIndex)->fileUrl().toLocalFile();
            GridManager::instance()->move(selectLocalFiles, current, row, col);
            setState(NoState);
            viewport()->update();
            return;

        }

        index = rootIndex();

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

    painter.setBrush(QColor(255, 0, 0, 0));
    if (model()) {
        for (int i = 0; i < d->colCount * d->rowCount; ++i) {
            auto  pos = d->indexCoordinate(i).position();
            auto x = pos.x() * d->cellWidth + d->viewMargins.left();
            auto y = pos.y() * d->cellHeight + d->viewMargins.top();
            auto rect =  QRect(x, y, d->cellWidth, d->cellHeight);

            int rowMode = pos.x() % 2;
            int colMode = pos.y() % 2;
            auto color = (colMode == rowMode) ? QColor(0, 0, 255, 32) : QColor(255, 0, 0, 32);
            painter.fillRect(rect, color);
        }
    }

    for (int i = 0; i < this->model()->rowCount(); ++i) {
        auto index = this->model()->index(i, 0) ;
        Q_ASSERT(index.isValid());
        option.rect = visualRect(index);
        QPainterPath path;
        path.addRect(option.rect.marginsAdded(QMargins(1, 1, 1, 1)));
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

        painter.fillPath(path, QColor(0, 0, 0, 16));
        this->itemDelegate()->paint(&painter, option, index);
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

    qDebug() << d->cellWidth << d->cellHeight;
    d->updateSize(event->size());
    GridManager::instance()->setCoordinateSize(d->colCount, d->rowCount);
    setContentsMargins(d->viewMargins);
    // todo restore
}

void CanvasGridView::contextMenuEvent(QContextMenuEvent *event)
{

    const QModelIndex &index = indexAt(event->pos());
    bool indexIsSelected = selectionModel()->isSelected(index);
    bool isEmptyArea = d->fileViewHelper->isEmptyArea(event->pos()) && !indexIsSelected;
    Qt::ItemFlags flags;

    if (isEmptyArea) {
        flags = model()->flags(rootIndex());

        if (!flags.testFlag(Qt::ItemIsEnabled)) {
            return;
        }
    } else {
        flags = model()->flags(index);

        if (!flags.testFlag(Qt::ItemIsEnabled)) {
            isEmptyArea = true;
            flags = rootIndex().flags();
        }
    }

    if (isEmptyArea) {
        itemDelegate()->hideNotEditingIndexWidget();
        clearSelection();
        showEmptyAreaMenu(flags);
    } else {
        if (!selectionModel()->isSelected(index)) {
            setCurrentIndex(index);
        }

        showNormalMenu(index, flags);
    }
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

    connect(delegate, &DStyledItemDelegate::commitData, this, [ = ](QWidget * editor) {
        qDebug() << editor;
    });
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

int CanvasGridView::selectedIndexCount() const
{
    return static_cast<const DFileSelectionModel *>(selectionModel())->selectedCount();
}

bool CanvasGridView::edit(const QModelIndex &index, QAbstractItemView::EditTrigger trigger, QEvent *event)
{
    DUrl fileUrl = model()->getUrlByIndex(index);

    if (fileUrl.isEmpty() || selectedIndexCount() > 1 || (trigger == SelectedClicked && DFMGlobal::keyShiftIsPressed())) {
        return false;
    }

    if (trigger == SelectedClicked) {
        QStyleOptionViewItem option = viewOptions();

        option.rect = visualRect(index);

        const QRect &file_name_rect = itemDelegate()->fileNameRect(option, index);

        if (!file_name_rect.contains(static_cast<QMouseEvent *>(event)->pos())) {
            return false;
        }
    }

    return false;

    return QAbstractItemView::edit(index, trigger, event);
}

void CanvasGridView::handleContextMenuAction(int action)
{
    switch (action) {
    case DisplaySettings:
        QProcess::startDetached("/usr/bin/dde-control-center",
                                QStringList() << "-s" << "dispaly");
        break;
    case CornerSettings:
        QProcess::startDetached("/usr/lib/deepin-daemon/dde-zone");
        break;
    case WallpaperSettings:
        QProcess::startDetached("/usr/lib/deepin-daemon/dde-wallpaper-chooser");

    case MenuAction::SelectAll:
        this->selectAll();
        break;
    case MenuAction::Property:
//        this->selectAll();
        break;
    }
}

void CanvasGridView::showEmptyAreaMenu(const Qt::ItemFlags &indexFlags)
{
    const QModelIndex &index = rootIndex();
    const DAbstractFileInfoPointer &info = model()->fileInfo(index);
    QVector<MenuAction> actions;
    actions << MenuAction::NewFolder << MenuAction::NewDocument
            << MenuAction::SortBy << MenuAction::Paste
            << MenuAction::SelectAll << MenuAction::Property
            << MenuAction::Separator;

    if (actions.isEmpty()) {
        return;
    }

    const QMap<MenuAction, QVector<MenuAction> > &subActions = info->subMenuActionList();

    QSet<MenuAction> disableList = DFileMenuManager::getDisableActionList(model()->getUrlByIndex(index));

    if (model()->state() != DFileSystemModel::Idle) {
        disableList << MenuAction::SortBy;
    }

    if (!indexFlags.testFlag(Qt::ItemIsEditable)) {
        disableList << MenuAction::NewDocument << MenuAction::NewFolder << MenuAction::Paste;
    }


    if (!model()->rowCount()) {
        disableList << MenuAction::SelectAll;
    }

    DFileMenu *menu = DFileMenuManager::genereteMenuByKeys(actions, disableList, true, subActions);

    DAction display(menu);
    display.setText(tr("Display Settings"));
    display.setData(DisplaySettings);
    menu->addAction(&display);

    DAction corner(menu);
    corner.setText(tr("Corner Settings"));
    corner.setData(CornerSettings);
    menu->addAction(&corner);

    DAction wallpaper(menu);
    wallpaper.setText(tr("Wallpaper Settings"));
    wallpaper.setData(WallpaperSettings);
    menu->addAction(&wallpaper);

//    DAction *tmp_action = menu->actionAt(fileMenuManger->getActionString(MenuAction::SortBy));
//    DFileMenu *sortBySubMenu = static_cast<DFileMenu*>(tmp_action ? tmp_action->menu() : Q_NULLPTR);

//    for (QAction *action : d->sortByActionGroup->actions()) {
//        d->sortByActionGroup->removeAction(action);
//    }

//    if (sortBySubMenu){
//        foreach (DAction* action, sortBySubMenu->actionList()) {
//            action->setActionGroup(d->sortByActionGroup);
//        }

//        DAction *action = sortBySubMenu->actionAt(model()->sortColumn());

//        if (action)
//            action->setChecked(true);
//    }

    DUrlList urls;
    urls.append(model()->rootUrl());

    DFMEvent event;
    event << model()->rootUrl();
    event << urls;
    event << winId();
    event << DFMEvent::FileView;
    menu->setEvent(event);

    connect(menu, &DFileMenu::triggered, this, [ = ](DAction * action) {
        if (!action->data().isValid()) {
            return;
        }

        auto menuAction = (action->data().toInt());
        qDebug() << menuAction;
        handleContextMenuAction(menuAction);
    });

    menu->exec();
    menu->deleteLater();
}

void CanvasGridView::showNormalMenu(const QModelIndex &index, const Qt::ItemFlags &indexFlags)
{
    if (!index.isValid()) {
        return;
    }

    DUrlList list;

    for (const QModelIndex &index : selectedIndexes()) {
        list << model()->getUrlByIndex(index);
    }

    DFileMenu *menu;

    const DAbstractFileInfoPointer &info = model()->fileInfo(index);
    QSet<MenuAction> disableList;

    if (!indexFlags.testFlag(Qt::ItemIsEditable)) {
        disableList << MenuAction::Cut << MenuAction::Rename << MenuAction::Remove << MenuAction::Delete;
    }

    if (list.length() == 1) {
        QVector<MenuAction> actions = info->menuActionList(DAbstractFileInfo::SingleFile);
//Vector(DFMGlobal::MenuAction(Open), DFMGlobal::MenuAction(OpenInNewWindow), DFMGlobal::MenuAction(OpenInNewTab), DFMGlobal::MenuAction(Separator), DFMGlobal::MenuAction(Cut), DFMGlobal::MenuAction(Copy), DFMGlobal::MenuAction(Rename), DFMGlobal::MenuAction(Compress), DFMGlobal::MenuAction(Share), DFMGlobal::MenuAction(CreateSymlink), DFMGlobal::MenuAction(SendToDesktop), DFMGlobal::MenuAction(AddToBookMark), DFMGlobal::MenuAction(OpenInTerminal), DFMGlobal::MenuAction(Delete), DFMGlobal::MenuAction(Separator), DFMGlobal::MenuAction(Property))
        qDebug() << actions;
        if (actions.contains(MenuAction::OpenInNewWindow)) {
            actions.remove(actions.indexOf(MenuAction::OpenInNewWindow));
        }
        if (actions.contains(MenuAction::OpenInNewTab)) {
            actions.remove(actions.indexOf(MenuAction::OpenInNewTab));
        }
        if (actions.contains(MenuAction::SendToDesktop)) {
            actions.remove(actions.indexOf(MenuAction::SendToDesktop));
        }
        if (actions.contains(MenuAction::AddToBookMark)) {
            actions.remove(actions.indexOf(MenuAction::AddToBookMark));
        }

        if (actions.isEmpty()) {
            return;
        }

        const QMap<MenuAction, QVector<MenuAction> > &subActions = info->subMenuActionList();
        disableList += DFileMenuManager::getDisableActionList(list);

        qDebug() << disableList;

        menu = DFileMenuManager::genereteMenuByKeys(actions, disableList, true, subActions);

        DAction *openWithAction = menu->actionAt(DFileMenuManager::getActionString(DFMGlobal::OpenWith));
        DFileMenu *openWithMenu = openWithAction ? qobject_cast<DFileMenu *>(openWithAction->menu()) : Q_NULLPTR;

        if (openWithMenu) {
//            QMimeType mimeType = info->mimeType();
//            QStringList recommendApps = mimeAppsManager->MimeApps.value(mimeType.name());

//            foreach (QString name, mimeType.aliases()) {
//                QStringList apps = mimeAppsManager->MimeApps.value(name);
//                foreach (QString app, apps) {
//                    if (!recommendApps.contains(app)){
//                        recommendApps.append(app);
//                    }
//                }
//            }

//            for (QAction *action : d->openWithActionGroup->actions()) {
//                d->openWithActionGroup->removeAction(action);
//            }

//            foreach (QString app, recommendApps) {
//                DAction* action = new DAction(mimeAppsManager->DesktopObjs.value(app).getLocalName(), 0);
//                action->setProperty("app", app);
//                action->setProperty("url", info->fileUrl());
//                openWithMenu->addAction(action);
//                d->openWithActionGroup->addAction(action);
//            }

//            DAction* action = new DAction(fileMenuManger->getActionString(MenuAction::OpenWithCustom), 0);
//            action->setData((int)MenuAction::OpenWithCustom);
//            openWithMenu->addAction(action);
        }
    } else {
        bool isAllCompressedFiles = true;

        foreach(DUrl url, list) {
            const DAbstractFileInfoPointer &fileInfo = DFileService::instance()->createFileInfo(url);

            if (!fileInfo->isActive()) {
                isAllCompressedFiles = false;
            }
        }

        QVector<MenuAction> actions;
        actions = info->menuActionList(DAbstractFileInfo::MultiFiles);

        if (actions.isEmpty()) {
            return;
        }

        if (isAllCompressedFiles) {
            int index = actions.indexOf(MenuAction::Compress);
            actions.insert(index + 1, MenuAction::Decompress);
            actions.insert(index + 2, MenuAction::DecompressHere);
        }

        const QMap<MenuAction, QVector<MenuAction> > subActions;
        disableList += DFileMenuManager::getDisableActionList(list);
        menu = DFileMenuManager::genereteMenuByKeys(actions, disableList, true, subActions);
    }

    DFMEvent event;

    event << info->fileUrl();
    event << list;
    event << winId();
    event << DFMEvent::FileView;

    menu->setEvent(event);
    menu->exec();
    menu->deleteLater();
}
