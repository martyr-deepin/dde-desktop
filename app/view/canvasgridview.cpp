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
#include <QApplication>
#include <QScreen>
#include <QAction>
#include <QDir>
#include <QStandardPaths>

#include <dthememanager.h>
#include <dscrollbar.h>
#include <dslider.h>
#include <anchors.h>
#include <DUtil>

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
#include "../presenter/gridmanager.h"
#include "../presenter/apppresenter.h"
#include "../config/config.h"

#include "canvasviewhelper.h"
#include "util/xcb/xcb.h"
#include "private/canvasviewprivate.h"

static const QString TrashRoot = "trash:///";

static inline bool isComputerFile(const DUrl &url)
{
#ifdef DDE_COMPUTER_TRASH
    return url.toString().endsWith("/dde-computer.desktop");
#else
    return url.toString().endsWith("/dde-computer.desktop");
#endif
}
static inline bool isTrashFile(const DUrl &url)
{
#ifdef DDE_COMPUTER_TRASH
    return url.toString().endsWith("/dde-trash.desktop");
#else
    Q_UNUSED(url);
    return false;
#endif
}
static inline bool isPersistFile(const DUrl &url)
{
#ifdef DDE_COMPUTER_TRASH
    return url.toString().endsWith("/dde-computer.desktop")
           || url.toString().endsWith("/dde-trash.desktop");
#else
    Q_UNUSED(url);
    return false;
#endif
}

static void startProcessDetached(const QString &program,
                                 const QStringList &arguments = QStringList(),
                                 QIODevice::OpenMode mode = QIODevice::ReadWrite)
{
    QProcess *process = new QProcess();
    process->start(program, arguments, mode);
    process->closeReadChannel(QProcess::StandardOutput);
    process->closeReadChannel(QProcess::StandardError);
    process->connect(process, static_cast < void(QProcess::*)(int) > (&QProcess::finished),
    process, [ = ](int) {
        process->deleteLater();
    });
}

DWIDGET_USE_NAMESPACE

CanvasGridView::CanvasGridView(QWidget *parent)
    : QAbstractItemView(parent), d(new CanvasViewPrivate)
{
    D_THEME_INIT_WIDGET(CanvasGridView);

    qRegisterMetaType<QList<DAbstractFileInfoPointer>>(QT_STRINGIFY(QList<DAbstractFileInfoPointer>));

    initUI();
    initConnection();
}

CanvasGridView::~CanvasGridView()
{

}

QRect CanvasGridView::visualRect(const QModelIndex &index) const
{
    auto url = model()->getUrlByIndex(index);
    auto gridPos = GridManager::instance()->position(url.toLocalFile());

    auto x = gridPos.x() * d->cellWidth + d->viewMargins.left();
    auto y = gridPos.y() * d->cellHeight + d->viewMargins.top();
    return QRect(x, y, d->cellWidth, d->cellHeight).marginsRemoved(d->cellMargins);
}

QModelIndex CanvasGridView::indexAt(const QPoint &point) const
{
    auto gridPos = gridAt(point);
    auto localFile =  GridManager::instance()->itemId(gridPos.x(), gridPos.y());
    auto rowIndex = model()->index(DUrl::fromLocalFile(localFile));
    QPoint pos = QPoint(point.x() + horizontalOffset(), point.y() + verticalOffset());
    auto list = itemPaintGeomertys(rowIndex);

    for (QModelIndex &index : itemDelegate()->hasWidgetIndexs()) {
        if (index == itemDelegate()->editingIndex()) {
            continue;
        }

        QWidget *widget = indexWidget(index);

        if (widget && widget->isVisible() && widget->geometry().contains(point)) {
            return index;
        }
    }

    for (const QRect &rect : list) {
        if (rect.contains(pos)) {
            return rowIndex;
        }
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
    auto headIndex = firstIndex();
    auto tailIndex = lastIndex();

    QModelIndex current = d->currentCursorIndex;
    if (!current.isValid() || !selections->isSelected(current)) {
        return headIndex;
    }
    auto url = model()->getUrlByIndex(current);
    auto pos = GridManager::instance()->position(url.toLocalFile());
    auto newCoord = Coordinate(pos);

    switch (cursorAction) {
    case MoveLeft:
        while (pos.x() >= 0) {
            newCoord = newCoord.moveLeft();
            pos = newCoord.position();
            if (!GridManager::instance()->isEmpty(pos.x(), pos.y())) {
                break;
            }
        }
        break;
    case MoveRight:
        while (pos.x() < d->colCount) {
            newCoord = newCoord.moveRight();
            pos = newCoord.position();
            if (!GridManager::instance()->isEmpty(pos.x(), pos.y())) {
                break;
            }
        }
        break;
    case MovePrevious:
    case MoveUp:
        while (pos.y() >= 0 && pos.x() >= 0) {
            newCoord = newCoord.moveUp();
            pos = newCoord.position();
            if (pos.y() < 0) {
                newCoord = Coordinate(pos.x() - 1, d->rowCount - 1);
                pos = newCoord.position();
            }
            if (!GridManager::instance()->isEmpty(pos.x(), pos.y())) {
                break;
            }
        }
        break;
    case MoveNext:
    case MoveDown:
        while (pos.y() < d->rowCount && pos.x() < d->colCount) {
            newCoord = newCoord.moveDown();
            pos = newCoord.position();
            if (pos.y() >= d->rowCount) {
                newCoord = Coordinate(pos.x() + 1, 0);
                pos = newCoord.position();
            }
            if (!GridManager::instance()->isEmpty(pos.x(), pos.y())) {
                break;
            }
        }
        break;
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

    auto localFile =  GridManager::instance()->itemId(pos.x(), pos.y());
    auto newIndex = model()->index(DUrl::fromLocalFile(localFile));
    if (newIndex.isValid()) {
        return newIndex;
    }

    return current;
}

QModelIndex CanvasGridView::moveCursor(QAbstractItemView::CursorAction cursorAction, Qt::KeyboardModifiers modifiers)
{
    auto current = d->currentCursorIndex;
    if (!current.isValid()) {
        current = firstIndex();
        d->currentCursorIndex = current;
        return current;
    }

    if (rectForIndex(current).isEmpty()) {
        qCritical() << "current never emptr" << current;
        d->currentCursorIndex = firstIndex();
        return d->currentCursorIndex;
    }

    QModelIndex index = moveCursorGrid(cursorAction, modifiers);

    if (index.isValid()) {
        d->currentCursorIndex = index;
        return index;
    }

    d->currentCursorIndex = current;
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
    setSelection(rect, command, false);
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
    if (dragEnabled() || event->buttons() != Qt::LeftButton
            || selectionMode() == NoSelection || selectionMode() == SingleSelection) {
        QAbstractItemView::mouseMoveEvent(event);
    }

    auto curPos = event->pos();
    QRect selectRect;

    if (d->selectFrame->isVisible()) {
        selectRect.setLeft(qMin(curPos.x(), d->lastPos.x()));
        selectRect.setTop(qMin(curPos.y(), d->lastPos.y()));
        selectRect.setRight(qMax(curPos.x(), d->lastPos.x()));
        selectRect.setBottom(qMax(curPos.y(), d->lastPos.y()));
        d->selectFrame->setGeometry(selectRect);
        d->selectFrame->raise();
    }


    if (d->selectFrame->isVisible()) {
        auto topLeftGridPos = gridAt(selectRect.topLeft());
        auto bottomRightGridPos = gridAt(selectRect.bottomRight());

//        if (topLeftGridPos != d->selectRect.topLeft()
//                || bottomRightGridPos != d->selectRect.bottomRight()) {
        d->selectRect = QRect(topLeftGridPos, bottomRightGridPos);
        auto flag = QItemSelectionModel::Current | QItemSelectionModel::ClearAndSelect;
        setSelection(selectRect, flag, true);
//    }
    }

}

void CanvasGridView::mousePressEvent(QMouseEvent *event)
{
    auto index = indexAt(event->pos());

    d->mousePressed = true;

    d->selectFrame->resize(1, 1);
    bool leftButtonPressed = event->button() == Qt::LeftButton;
    bool showSelectFrame = leftButtonPressed;
    showSelectFrame &= !index.isValid();
    d->selectFrame->setVisible(showSelectFrame);
    d->lastPos = event->pos();

    bool isEmptyArea = !index.isValid();

    if (isEmptyArea) {
        if (!DFMGlobal::keyCtrlIsPressed() && !DFMGlobal::keyShiftIsPressed()) {
            itemDelegate()->hideNotEditingIndexWidget();
            QAbstractItemView::setCurrentIndex(QModelIndex());
            clearSelection();;
        }
    }

    d->beforeMoveSelection = selectionModel()->selection();

    QAbstractItemView::mousePressEvent(event);

    if (leftButtonPressed) {
        d->currentCursorIndex = index;
    }
//    selectionModel()->setCurrentIndex(d->currentCursorIndex, QItemSelectionModel::ToggleCurrent);
}

void CanvasGridView::mouseReleaseEvent(QMouseEvent *event)
{
    d->mousePressed = false;
    d->selectFrame->setVisible(false);
    if (dragEnabled()) {
        return QAbstractItemView::mouseReleaseEvent(event);
    }

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

void CanvasGridView::wheelEvent(QWheelEvent *event)
{
    if (DFMGlobal::keyCtrlIsPressed()) {
        if (event->angleDelta().y() > 0) {
            increaseIcon();
        } else {
            decreaseIcon();
        }

        event->accept();
    }
}

void CanvasGridView::keyPressEvent(QKeyEvent *event)
{
    QMap<QString, DUrl> selectUrls;
    bool canDeleted = true;
    for (const QModelIndex &index : selectionModel()->selectedIndexes()) {
        auto url = model()->getUrlByIndex(index);
        if (isPersistFile(url)) {
            canDeleted = false;
        }
        selectUrls.insert(url.toString(), url);
    }

    DFMEvent fmevent;

    fmevent << selectUrls.values();
    fmevent << DFMEvent::FileView;
    fmevent << winId();
    fmevent << model()->rootUrl();

    switch (event->modifiers()) {
    case Qt::NoModifier:
    case Qt::KeypadModifier:
        switch (event->key()) {
        case Qt::Key_Return:
        case Qt::Key_Enter:
            if (!itemDelegate()->editingIndex().isValid()) {
//  TODO:              appController->actionOpen(fmevent);
                return;
            }
            break;
        case Qt::Key_F1:
//TODO:            appController->actionHelp(fmevent);
            return;
        case Qt::Key_F5:
            model()->refresh();
            return;
        case Qt::Key_Delete:
            if (canDeleted) {
                DFileService::instance()->moveToTrash(fmevent);
            }
            break;
        case Qt::Key_End:
//            if (urls.isEmpty()) {
//TODO:
//                setCurrentIndex(model()->index(count() - 1, 0));
            return;
//            }
        default: break;
        }
        break;

    case Qt::ShiftModifier:
        if (event->key() == Qt::Key_Delete) {
            if (fmevent.fileUrlList().isEmpty() || !canDeleted) {
                return;
            }

            DFileService::instance()->deleteFiles(fmevent);

            return;
        } else if (event->key() == Qt::Key_T) {
// Open           appController->actionOpenInTerminal(fmevent);
            return;
        }
        break;

    case Qt::ControlModifier:
        if (event->key() == Qt::Key_Minus) {
            decreaseIcon();
            return;
        }
        break;

    case Qt::ControlModifier | Qt::ShiftModifier:

        if (event->key() == Qt::Key_N) {
            if (itemDelegate()->editingIndex().isValid()) {
                return;
            }
//            clearSelection();
//            appController->actionNewFolder(fmevent);
            return;
        }

        if (event->key() == Qt::Key_Question) {
//            appController->actionShowHotkeyHelp(fmevent);
            return;
        }
        if (event->key() == Qt::Key_Plus) {
            increaseIcon();
            return;
        }
        break;

    default: break;
    }

    QAbstractItemView::keyPressEvent(event);
}

void CanvasGridView::dragEnterEvent(QDragEnterEvent *event)
{
//    for (const DUrl &url : event->mimeData()->urls()) {
//        qDebug() << url;
//        const DAbstractFileInfoPointer &fileInfo = DFileService::instance()->createFileInfo(url);
//        if (!fileInfo->isWritable() || isComputerFile(url.toString())) {
//            event->ignore();
//            return;
//        }
//    }

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
            if (!fileInfo->isDir() &&  !isTrashFile(fileInfo->fileUrl())) {
                d->dragMoveHoverIndex = QModelIndex();
            } else if (!fileInfo->supportedDropActions().testFlag(event->dropAction())
                       && !isTrashFile(fileInfo->fileUrl())) {
                d->dragMoveHoverIndex = QModelIndex();
                event->ignore();
            }
        }
    } else {
        event->accept();
        event->setDropAction(Qt::MoveAction);
    }

    update();

//    if ((event->source() != this || !(event->possibleActions() & Qt::MoveAction))) {

//        qDebug() << dragDropMode();
//        QAbstractItemView::dragMoveEvent(event);
//    }
}

void CanvasGridView::dragLeaveEvent(QDragLeaveEvent *event)
{
    d->dragMoveHoverIndex = QModelIndex();
    QAbstractItemView::dragLeaveEvent(event);
}

void CanvasGridView::dropEvent(QDropEvent *event)
{
    d->dragMoveHoverIndex = QModelIndex();
//    d->dragMoveHoverIndex = QModelIndex();

    QModelIndex targetIndex = indexAt(event->pos());

    QStringList selectLocalFiles;
    auto selects = selectionModel()->selectedIndexes();
    bool canMove = true;
    for (auto index : selects) {
        auto info = model()->fileInfo(index);
        if (isPersistFile(info->fileUrl())) {
            canMove = false;
        }
        if (targetIndex == index) {
            canMove = false;
        }
        selectLocalFiles << info->fileUrl().toLocalFile();
    }

    DAbstractFileInfoPointer targetInfo = model()->fileInfo(indexAt(event->pos()));
    if (!targetInfo) {
        targetInfo = model()->fileInfo(rootIndex());
    }

    if (event->source() == this && !DFMGlobal::keyCtrlIsPressed()) {
        if (!canMove) {
            event->setDropAction(Qt::IgnoreAction);
        } else {
            event->setDropAction(Qt::MoveAction);
        }

    } else {
        if (targetInfo && !targetInfo->supportedDropActions().testFlag(event->dropAction())) {
            QList<Qt::DropAction> actions;

            actions.reserve(3);
            actions << Qt::CopyAction << Qt::MoveAction << Qt::LinkAction;

            for (Qt::DropAction action : actions) {
                if (event->possibleActions().testFlag(action)
                        && targetInfo->supportedDropActions().testFlag(action)) {
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
        QModelIndex dropIndex = indexAt(gridRectAt(event->pos()).center());
        if (!dropIndex.isValid()) {
            if (event->source() == this) {
                auto point = event->pos();
                auto row = (point.x() - d->viewMargins.left()) / d->cellWidth;
                auto col = (point.y() - d->viewMargins.top()) / d->cellHeight;
                auto current = model()->fileInfo(d->currentCursorIndex)->fileUrl().toLocalFile();
                GridManager::instance()->move(selectLocalFiles, current, row, col);
                setState(NoState);
                itemDelegate()->hideNotEditingIndexWidget();
                update();
                return;
            }
        }

        if (!targetIndex.isValid()) {
            targetIndex = rootIndex();
        }


        if (model()->supportedDropActions() & event->dropAction() && model()->flags(targetIndex) & Qt::ItemIsDropEnabled) {
            const Qt::DropAction action = dragDropMode() == InternalMove ? Qt::MoveAction : event->dropAction();
            if (model()->dropMimeData(event->mimeData(), action, targetIndex.row(), targetIndex.column(), targetIndex)) {
                if (action != event->dropAction()) {
                    event->setDropAction(action);
                    event->accept();
                } else {
                    event->acceptProposedAction();
                }
            }
        }

        if (isTrashFile(targetInfo->fileUrl()) && canMove) {
            DFMEvent fmevent;

            fmevent << this->selectedUrls();
            fmevent << DFMEvent::FileView;
            fmevent << winId();
            fmevent << model()->rootUrl();
            DFileService::instance()->moveToTrash(fmevent);
        }

        setState(NoState);
        viewport()->update();
    }
}
#include <QApplication>

void CanvasGridView::paintEvent(QPaintEvent *)
{
    QPainter painter(viewport());
    painter.setRenderHints(QPainter::Antialiasing | QPainter::HighQualityAntialiasing);

    auto option = viewOptions();
    option.font.setPixelSize(14);
    qApp->setFont(option.font);

    const QModelIndex current = d->currentCursorIndex;
    const QAbstractItemModel *itemModel = this->model();
    const DFileSelectionModel *selections = this->selectionModel();
    const bool focus = (hasFocus() || viewport()->hasFocus()) && current.isValid();
    const QStyle::State state = option.state;
    const QAbstractItemView::State viewState = this->state();
    const bool enabled = (state & QStyle::State_Enabled) != 0;

    painter.setBrush(QColor(255, 0, 0, 0));

#ifdef SHOW_GRID
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
#endif

    if (d->dragMoveHoverIndex.isValid() && d->dragMoveHoverIndex != d->currentCursorIndex) {
        QPainterPath path;
        auto lastRect = visualRect(d->dragMoveHoverIndex);
        path.addRoundRect(lastRect, 4, 4);
        painter.fillPath(path, QColor(43, 167, 248, 0.30 * 255));
        painter.strokePath(path, QColor(30, 126, 255, 0.20 * 255));
    }

    // TODO draw later
    for (int i = 0; i < this->model()->rowCount(); ++i) {
        auto index = this->model()->index(i, 0) ;
        Q_ASSERT(index.isValid());
        option.rect = visualRect(index);
        QPainterPath path;
        path.addRect(option.rect.marginsAdded(QMargins(1, 1, 1, 1)));
        option.rect.marginsRemoved(QMargins(5, 5, 5, 5));
        option.state = state;
        if (selections && selections->isSelected(index)) {
            option.state |= QStyle::State_Selected;
        }
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

        this->itemDelegate()->paint(&painter, option, index);
    }

//    if (d->currentCursorIndex.isValid()) {
//        auto lastRects = itemPaintGeomertys(d->currentCursorIndex);
//        if (lastRects.length() >= 2) {
//            auto lastRect = lastRects.at(1);
//            lastRect = visualRect(d->currentCursorIndex);

//            QPainterPath path;
//            path.addRoundRect(lastRect, 6, 6);
//            QPen pen(QColor(255, 255, 255, 255), 2.0);
//            painter.strokePath(path, pen);
//        }
//    }


}

void CanvasGridView::resizeEvent(QResizeEvent * /*event*/)
{
    updateCanvas();
    // todo restore
}

void CanvasGridView::focusInEvent(QFocusEvent *event)
{
    QAbstractItemView::focusInEvent(event);
    itemDelegate()->commitDataAndCloseActiveEditor();

    /// set menu actions filter
    DFileMenuManager::setActionWhitelist(QSet<MenuAction>());
    DFileMenuManager::setActionBlacklist(QSet<MenuAction>());

    /// set file operator function filter
    DFileService::FileOperatorTypes fileOperatorlist;
    DFileService::instance()->setFileOperatorWhitelist(fileOperatorlist);
    DFileService::instance()->setFileOperatorBlacklist(fileOperatorlist);
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
}

bool CanvasGridView::setCurrentUrl(const DUrl &url)
{
    DUrl fileUrl = url;
    const DAbstractFileInfoPointer &info = DFileService::instance()->createFileInfo(fileUrl);
    if (!info) {
        qDebug() << "This scheme isn't support";
        return false;
    }

    const DUrl &checkUrl = currentUrl();

    if (checkUrl == fileUrl) {
        return false;
    }

    QModelIndex index = model()->setRootUrl(fileUrl);
    setRootIndex(index);

    if (!model()->canFetchMore(index)) {
        // TODO: updateContentLabel
        qDebug() << "TODO: updateContentLabel()";
    }

    if (focusWidget() && focusWidget()->window() == window() && fileUrl.isLocalFile()) {
        QDir::setCurrent(fileUrl.toLocalFile());
    }

    QAbstractItemView::setCurrentIndex(QModelIndex());

    model()->setFilters(model()->filters() & ~QDir::Hidden);

    if (d->filesystemWatcher) {
        d->filesystemWatcher->deleteLater();
    }

    d->filesystemWatcher = new DFileSystemWatcher(QStringList() << fileUrl.toLocalFile());

    connect(d->filesystemWatcher, &DFileSystemWatcher::fileCreated,
    this, [ = ](const QString & path, const QString & name) {
//        qDebug() << path << name;
        const QString &filePath = path + QDir::separator() + name;
        if (filePath.endsWith(".desktop")) {
            d->filesystemWatcher->addPath(filePath);
        }
    });

    connect(d->filesystemWatcher, &DFileSystemWatcher::fileClosed,
    this, [ = ](const QString & path, const QString & /*name*/) {
//        qDebug() << path << name;
        auto index = model()->index(DUrl::fromLocalFile(path));
        auto info = model()->fileInfo(index);
        if (info) {
            info->refresh();
        }
    });

    connect(d->filesystemWatcher, &DFileSystemWatcher::fileDeleted,
    this, [ = ](const QString & path, const QString & /*name*/) {
//        qDebug() << path << name;
        d->filesystemWatcher->removePath(path);
    });
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

    QDir rootDir(url.toLocalFile());
    return setCurrentUrl(url);
}

const DUrlList CanvasGridView::selectedUrls() const
{
    auto selects = selectionModel()->selectedIndexes();
    DUrlList urls;
    for (auto index : selects) {
        auto info = model()->fileInfo(index);
        urls << info->fileUrl();
    }
    return urls;
}

void CanvasGridView::select(const QList<DUrl> &list)
{
    QItemSelection selection;
    for (auto &url : list) {
        auto index = model()->index(url);
        QItemSelectionRange selectionRange(index);
        if (!selection.contains(index)) {
            selection.push_back(selectionRange);
        }
        QAbstractItemView::selectionModel()->select(selection, QItemSelectionModel::Select);
    }
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

    if (QWidget *w = indexWidget(index)) {
        Qt::ItemFlags flags = model()->flags(index);
        if (((flags & Qt::ItemIsEditable) == 0) || ((flags & Qt::ItemIsEnabled) == 0)) {
            return false;
        }
        if (state() == QAbstractItemView::EditingState) {
            return false;
        }
        if (trigger == QAbstractItemView::AllEditTriggers) { // force editing
            d->fileViewHelper->triggerEdit(index);
            return true;
        }
        if ((trigger & editTriggers()) == QAbstractItemView::SelectedClicked
                && !selectionModel()->isSelected(index)) {
            return false;
        }

        if (trigger & editTriggers()) {
            w->setFocus();
            d->fileViewHelper->triggerEdit(index);
            return true;
        }
    }

    bool tmp = QAbstractItemView::edit(index, trigger, event);

    if (tmp) {
        d->fileViewHelper->triggerEdit(index);
    }

    return tmp;
}

void CanvasGridView::initUI()
{
    setAttribute(Qt::WA_TranslucentBackground);
    viewport()->setAttribute(Qt::WA_TranslucentBackground);
    Xcb::XcbMisc::instance().set_window_type(winId(), Xcb::XcbMisc::Desktop);

    setGeometry(qApp->primaryScreen()->geometry());
    d->canvasRect = qApp->primaryScreen()->availableGeometry();

    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setAcceptDrops(true);
    setDragDropMode(QAbstractItemView::DragDrop);
    setEditTriggers(QAbstractItemView::EditKeyPressed | QAbstractItemView::SelectedClicked);

    d->selectFrame = new QFrame(this);
    d->selectFrame->setVisible(false);
    d->selectFrame->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    d->selectFrame->setObjectName("SelectRect");

    d->fileViewHelper = new CanvasViewHelper(this);

    setModel(new DFileSystemModel(d->fileViewHelper));
    model()->setEnabledSort(false);

    setSelectionModel(new DFileSelectionModel(model(), this));
    auto delegate = new DIconItemDelegate(d->fileViewHelper);
    delegate->setEnabledTextShadow(true);
    setItemDelegate(delegate);

    qobject_cast<DIconItemDelegate *>(itemDelegate())->setFocusTextBackgroundBorderColor(Qt::white);

    auto settings = Config::instance()->settings();
    settings->beginGroup(Config::groupGeneral);
    if (settings->contains(Config::keyIconLevel)) {
        auto iconSizeLevel = settings->value(Config::keyIconLevel).toInt();
        itemDelegate()->setIconSizeByIconSizeLevel(iconSizeLevel);
        qDebug() << "current icon size level" << itemDelegate()->iconSizeLevel();
    } else {
        itemDelegate()->setIconSizeByIconSizeLevel(0);
    }
    settings->endGroup();
}

void CanvasGridView::initConnection()
{
    auto syncTimer = new QTimer(this);
    syncTimer->setInterval(2000);
    connect(syncTimer, &QTimer::timeout, this, [ = ]() {
        this->update();
    });
    syncTimer->start();

    connect(qApp->primaryScreen(), &QScreen::availableGeometryChanged,
    this, [ = ](const QRect & geometry) {
        qDebug() << "Screen geometry changed" << geometry;
        setGeometry(qApp->primaryScreen()->geometry());
        d->canvasRect = qApp->primaryScreen()->availableGeometry();

        updateCanvas();
        repaint();
    });

    connect(this->model(), &QAbstractItemModel::rowsAboutToBeInserted,
    this, [ = ](const QModelIndex & parent, int first, int last) {
        if (!d->filesystemWatcher) {
            return;
        }

        QStringList files;
        for (int i = first; i <= last; ++i) {
            auto index = model()->index(i, 0, parent);
            auto localFile = model()->getUrlByIndex(index).toLocalFile();

            files << localFile;
        }
//        qDebug() << "add watch" << files;
        d->filesystemWatcher->addPaths(files);
    });

    connect(this->model(), &QAbstractItemModel::rowsInserted,
    this, [ = ](const QModelIndex & parent, int first, int last) {
//        qDebug() << parent << first << last;

        if (!GridManager::instance()->isInited()) {
            QStringList files;
            for (int i = first; i <= last; ++i) {
                auto index = model()->index(i, 0, parent);
                auto localFile = model()->getUrlByIndex(index).toLocalFile();
                files << localFile;
            }
            qDebug() << "init GridManager cells";
            GridManager::instance()->initProfile(files);
            return;
        }

        for (int i = first; i <= last; ++i) {
            auto index = model()->index(i, 0, parent);
            auto localFile = model()->getUrlByIndex(index).toLocalFile();
            qDebug() << "add" << localFile;
            GridManager::instance()->add(localFile);
        }
    });
    connect(this->model(), &QAbstractItemModel::rowsAboutToBeRemoved,
    this, [ = ](const QModelIndex & parent, int first, int last) {

        qDebug() << model()->getUrlByIndex(parent).toLocalFile();
        for (int i = first; i <= last; ++i) {
            auto index = model()->index(i, 0, parent);
            if (d->currentCursorIndex == index) {
                d->currentCursorIndex  = QModelIndex();
                selectionModel()->setCurrentIndex(QModelIndex(), QItemSelectionModel::Clear);
                setCurrentIndex(QModelIndex());
            }

            auto localFile = model()->getUrlByIndex(index).toLocalFile();
            qDebug() << "rowsAboutToBeRemoved" << localFile;
            GridManager::instance()->remove(localFile);
        }
    });
    connect(this->model(), &QAbstractItemModel::rowsRemoved,
    this, [ = ](const QModelIndex & /*parent*/, int /*first*/, int /*last*/) {
        if (GridManager::instance()->autoAlign()) {
            GridManager::instance()->reAlign();
        }
    });
    connect(this->model(), &QAbstractItemModel::dataChanged,
            this, [ = ](const QModelIndex & topLeft,
                        const QModelIndex & bottomRight,
    const QVector<int> &roles) {
        qDebug() << "dataChanged" << topLeft << bottomRight << roles;

        if (d->resortCount > 0) {
            qDebug() << "resort desktop icons";
            model()->setEnabledSort(false);
            d->resortCount--;
            GridManager::instance()->clear();
            QStringList list;
            for (int i = 0; i < model()->rowCount(); ++i) {
                auto index = model()->index(i, 0);
                auto localFile = model()->getUrlByIndex(index).toLocalFile();
                list << localFile;
            }
            for (auto lf : list) {
                GridManager::instance()->add(lf);
            }
            GridManager::instance()->reAlign();
        }

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

        QFileInfo info(url.toLocalFile());
        if (info.isDir()) {
            QProcess::startDetached("gvfs-open", QStringList() << url.toLocalFile());
        } else {
            DFileService::instance()->openFile(url);
        }
    }, Qt::QueuedConnection);


    connect(this, &CanvasGridView::autoAlignToggled,
            Presenter::instance(), &Presenter::onAutoAlignToggled);
    connect(this, &CanvasGridView::sortRoleChanged,
            Presenter::instance(), &Presenter::onSortRoleChanged);
    connect(this, &CanvasGridView::changeIconLevel,
            Presenter::instance(), &Presenter::OnIconLevelChanged);
}

void CanvasGridView::updateCanvas()
{
    auto outRect = qApp->primaryScreen()->geometry();
    auto inRect = qApp->primaryScreen()->availableGeometry();
    auto itemSize = itemDelegate()->sizeHint(QStyleOptionViewItem(), QModelIndex());

    QMargins geometryMargins;
    geometryMargins.setLeft(inRect.left() - outRect.left());
    geometryMargins.setRight(outRect.right() - inRect.right());
    geometryMargins.setTop(inRect.top() - outRect.top());
    geometryMargins.setBottom(outRect.bottom() - inRect.bottom());

    d->updateCanvasSize(d->canvasRect.size(), geometryMargins, itemSize);
    GridManager::instance()->updateGridSize(d->colCount, d->rowCount);

    repaint();
}

void CanvasGridView::increaseIcon()
{
    // TODO: 3 is 128*128, 0,1,2,3
    if (itemDelegate()->iconSizeLevel() >= 3) {
        return;
    }
    itemDelegate()->increaseIcon();
    emit this->changeIconLevel(itemDelegate()->iconSizeLevel());
    updateCanvas();
}

void CanvasGridView::decreaseIcon()
{
    itemDelegate()->decreaseIcon();
    emit this->changeIconLevel(itemDelegate()->iconSizeLevel());
    updateCanvas();
}

inline QPoint CanvasGridView::gridAt(const QPoint &pos) const
{
    auto row = (pos.x() - d->viewMargins.left()) / d->cellWidth;
    auto col = (pos.y() - d->viewMargins.top()) / d->cellHeight;
    return QPoint(row, col);
}

inline QRect CanvasGridView::gridRectAt(const QPoint &pos) const
{
    auto row = (pos.x() - d->viewMargins.left()) / d->cellWidth;
    auto col = (pos.y() - d->viewMargins.top()) / d->cellHeight;

    auto x = row * d->cellWidth + d->viewMargins.left();
    auto y = col * d->cellHeight + d->viewMargins.top();
    return QRect(x, y, d->cellWidth, d->cellHeight).marginsRemoved(d->cellMargins);
}

inline QList<QRect> CanvasGridView::itemPaintGeomertys(const QModelIndex &index) const
{
    QStyleOptionViewItem option = viewOptions();
    option.rect = visualRect(index);
    return itemDelegate()->paintGeomertys(option, index);
}

inline QModelIndex CanvasGridView::firstIndex()
{
    auto localFile = GridManager::instance()->firstItemId();
    return model()->index(DUrl::fromLocalFile(localFile));
}

inline QModelIndex CanvasGridView::lastIndex()
{
    auto localFile = GridManager::instance()->lastItemId();
    return model()->index(DUrl::fromLocalFile(localFile));
}

void CanvasGridView::setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags command, bool byIconRect)
{
    auto selectRect = rect.normalized();
    auto topLeftGridPos = gridAt(selectRect.topLeft());
    auto bottomRightGridPos = gridAt(selectRect.bottomRight());

    QItemSelection selection;
    if (DFMGlobal::keyShiftIsPressed()) {
        selection = selectionModel()->selection();
    }

    if (!d->mousePressed && d->currentCursorIndex.isValid()) {
        QItemSelectionRange selectionRange(d->currentCursorIndex);
        selection.push_back(selectionRange);
        QAbstractItemView::selectionModel()->select(selection, command);
        return;
    }

    if (d->mousePressed && DFMGlobal::keyShiftIsPressed()) {
        auto clickIndex = indexAt(d->lastPos);
        if (clickIndex.isValid()) {
            auto clickedPoint = visualRect(clickIndex).center();
            auto lastPoint = visualRect(d->currentCursorIndex).center();
            if (!d->currentCursorIndex.isValid()) {
                lastPoint = clickedPoint + QPoint(1, 1);
            }
            selectRect = QRect(clickedPoint, lastPoint).normalized();
            topLeftGridPos = gridAt(selectRect.topLeft());
            bottomRightGridPos = gridAt(selectRect.bottomRight());
        } else {
            if (!d->selectFrame->isVisible()) {
                return;
            }
            selection = d->beforeMoveSelection;
            topLeftGridPos = d->selectRect.topLeft();
            bottomRightGridPos = d->selectRect.bottomRight();
        }

    }

    for (auto x = topLeftGridPos.x(); x <= bottomRightGridPos.x(); ++x) {
        for (auto y = topLeftGridPos.y(); y <= bottomRightGridPos.y(); ++y) {
            auto localFile = GridManager::instance()->itemId(x, y);
            if (localFile.isEmpty()) {
                continue;
            }
            auto index = model()->index(DUrl::fromLocalFile(localFile));
            auto list = itemPaintGeomertys(index);
            for (const QRect &r : list) {
                if (selectRect.intersects(r)) {
                    QItemSelectionRange selectionRange(index);
                    if (!selection.contains(index)) {
                        selection.push_back(selectionRange);
                    }
                    break;
                }
                if (byIconRect) {
                    break;
                }
            }
        }
    }

    QAbstractItemView::selectionModel()->select(selection, command);
}

void CanvasGridView::handleContextMenuAction(int action)
{
    bool changeSort  = false;

    switch (action) {
    case DisplaySettings:
        startProcessDetached("/usr/bin/dde-control-center",
                             QStringList() << "-s" << "dispaly");
        break;
    case CornerSettings:
        startProcessDetached("/usr/lib/deepin-daemon/dde-zone");
        break;
    case WallpaperSettings:
        startProcessDetached("/usr/lib/deepin-daemon/dde-wallpaper-chooser");
        break;
    case MenuAction::SelectAll:
        this->selectAll();
        break;
    case FileManagerProperty: {
        QStringList localFiles;
        localFiles << currentUrl().toLocalFile();
        startProcessDetached("/usr/bin/dde-property-dialog",
                             localFiles);
        break;
    }
    case AutoSort:
        emit autoAlignToggled();
        break;

    case MenuAction::Name:
    case MenuAction::Size:
    case MenuAction::Type:
    case MenuAction::LastModifiedDate:
        changeSort = true;
        break;

    default:
        qDebug() << action;
    }

    if (changeSort) {
        model()->setEnabledSort(true);
        d->resortCount++;
        QMap<int, int> sortActions;
        sortActions.insert(MenuAction::Name, DFileSystemModel::FileDisplayNameRole);
        sortActions.insert(MenuAction::Size, DFileSystemModel::FileSizeRole);
        sortActions.insert(MenuAction::Type, DFileSystemModel::FileMimeTypeRole);
        sortActions.insert(MenuAction::LastModifiedDate, DFileSystemModel::FileLastModifiedRole);

        int             sortRole    = sortActions.value(action);
        Qt::SortOrder   sortOrder   = model()->sortOrder() == Qt::AscendingOrder ?
                                      Qt::DescendingOrder : Qt::AscendingOrder;

        model()->setSortRole(sortRole, sortOrder);
        model()->sort();
        emit sortRoleChanged(sortRole, sortOrder);
    }
}

void CanvasGridView::showEmptyAreaMenu(const Qt::ItemFlags &indexFlags)
{
    const QModelIndex &index = rootIndex();
    const DAbstractFileInfoPointer &info = model()->fileInfo(index);
    QVector<MenuAction> actions;
    actions << MenuAction::NewFolder << MenuAction::NewDocument
            << MenuAction::SortBy << MenuAction::Paste
            << MenuAction::SelectAll << MenuAction::OpenInTerminal
            << MenuAction::Property << MenuAction::Separator;

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
    if (!menu) {
        return;
    }

    auto *pasteAction = menu->actionAt(DFileMenuManager::getActionString(MenuAction::Paste));
    QAction autoSort(menu);
    autoSort.setText(tr("Auto arrangement"));
    autoSort.setData(AutoSort);
    autoSort.setCheckable(true);
    autoSort.setChecked(GridManager::instance()->autoAlign());
    menu->insertAction(pasteAction, &autoSort);

    auto *propertyAction = menu->actionAt(DFileMenuManager::getActionString(MenuAction::Property));
    QAction property(menu);
    property.setText(tr("Property"));
    property.setData(FileManagerProperty);
    menu->insertAction(propertyAction, &property);

//    QAction *sortByAction = menu->actionAt(DFileMenuManager::getActionString(MenuAction::SortBy));
//    DFileMenu *sortBySubMenu = static_cast<DFileMenu *>(sortByAction ? sortByAction->menu() : Q_NULLPTR);
//    if (sortBySubMenu) {
//        QMap<int, MenuAction> sortActions;
//        sortActions.insert(DFileSystemModel::FileDisplayNameRole, MenuAction::Name);
//        sortActions.insert(DFileSystemModel::FileSizeRole, MenuAction::Size);
//        sortActions.insert(DFileSystemModel::FileMimeTypeRole, MenuAction::Type);
//        sortActions.insert(DFileSystemModel::FileLastModifiedRole, MenuAction::LastModifiedDate);

//        auto sortRole = model()->sortRole();
//        auto sortMenuAction = sortActions.value(sortRole);

//        QAction *sortRoleAction = sortBySubMenu->actionAt(DFileMenuManager::getActionString(sortMenuAction));

//        sortRoleAction->setChecked(d->autoSort);
//}

    QList<QAction *>  pluginActions  = DFileMenuManager::loadEmptyAreaPluginMenu(menu, model()->rootUrl());
    QList<QAction *>  extensionActions = DFileMenuManager::loadEmptyAreaExtensionMenu(menu, model()->rootUrl());

    if (pluginActions.count() > 0) {
        QAction *separator = new QAction(menu);
        separator->setSeparator(true);
        menu->insertAction(pluginActions.at(0), separator);
    }

    QAction display(menu);
    display.setText(tr("Display Settings"));
    display.setData(DisplaySettings);
    menu->addAction(&display);

    QAction corner(menu);
    corner.setText(tr("Corner Settings"));
    corner.setData(CornerSettings);
    menu->addAction(&corner);

    QAction wallpaper(menu);
    wallpaper.setText(tr("Set Wallpaper"));
    wallpaper.setData(WallpaperSettings);
    menu->addAction(&wallpaper);

    menu->removeAction(propertyAction);

    DUrlList urls;
    urls.append(model()->rootUrl());

    DFMEvent event;
    event << model()->rootUrl();
    event << urls;
    event << winId();
    event << DFMEvent::FileView;
    menu->setEvent(event);

    connect(menu, &DFileMenu::triggered, this, [ = ](QAction * action) {
        qDebug() << action->data();
        if (!action->data().isValid()) {
            return;
        }
        handleContextMenuAction(action->data().toInt());
    });

    menu->exec();
    menu->deleteLater();
}


DFileMenu *CanvasGridView::createNormalMenu(const DUrl &currentUrl, const DUrlList &urlList)
{
    bool useCustomMenu = false;

    for (auto &url : urlList) {
        if (isPersistFile(url)) {
            useCustomMenu  = true;
            break;
        }
    }
    if (isPersistFile(currentUrl)) {
        useCustomMenu  = true;
    }

    if (!useCustomMenu) {
        return nullptr;
    }

    QSet<MenuAction> disableList;
    QSet<MenuAction> unusedList;

    disableList << MenuAction::Cut << MenuAction::Rename << MenuAction::Remove << MenuAction::Delete;

    unusedList << MenuAction::OpenInNewWindow
               << MenuAction::OpenInNewTab
               << MenuAction::Compress
               << MenuAction::SendToDesktop
               << MenuAction::Cut
               << MenuAction::Copy
               << MenuAction::Rename
               << MenuAction::Remove
               << MenuAction::AddToBookMark
               << MenuAction::OpenWith;

    DFileMenu *menu = DFileMenuManager::createNormalMenu(currentUrl, urlList, disableList, unusedList, -1);
    return menu;
}

void CanvasGridView::showNormalMenu(const QModelIndex &index, const Qt::ItemFlags &indexFlags)
{
    if (!index.isValid()) {
        return;
    }

    bool showProperty = true;
    DUrlList list;
    for (const QModelIndex &index : selectedIndexes()) {
        const DAbstractFileInfoPointer &info = model()->fileInfo(index);
        if (isComputerFile(info->fileUrl())) {
            showProperty = false;
        }

        list << info->fileUrl();
    }

    const DAbstractFileInfoPointer &info = model()->fileInfo(index);

    QSet<MenuAction> disableList;
    QSet<MenuAction> unusedList;

    if (!indexFlags.testFlag(Qt::ItemIsEditable)) {
        disableList << MenuAction::Cut << MenuAction::Rename << MenuAction::Remove << MenuAction::Delete;
    }

    if (list.length() == 1) {
        unusedList << MenuAction::OpenInNewWindow
                   << MenuAction::OpenInNewTab
                   << MenuAction::SendToDesktop
                   << MenuAction::AddToBookMark;
    } else {
        unusedList << MenuAction::SendToDesktop;
    }

    DFileMenu *menu = createNormalMenu(info->fileUrl(), list);

    if (!menu) {
        menu = DFileMenuManager::createNormalMenu(info->fileUrl(), list, disableList, unusedList, -1);
    }

    if (!menu) {
        return;
    }

    auto *propertyAction = menu->actionAt(DFileMenuManager::getActionString(MenuAction::Property));
    if (propertyAction) {
        menu->removeAction(propertyAction);
    }

    qDebug() << showProperty;
    QAction property(menu);
    if (showProperty) {
        property.setText(tr("Property"));
        property.setData(FileManagerProperty);
        menu->addAction(&property);
    }

    DFMEvent event;

    event << info->fileUrl();
    event << list;
    event << winId();
    event << DFMEvent::FileView;

    menu->setEvent(event);

    connect(menu, &DFileMenu::triggered, this, [ = ](QAction * action) {
        if (!action->data().isValid()) {
            return;
        }

        switch (action->data().toInt()) {
        case MenuAction::Open: {
            // TODO: Workaround
            for (auto &url : list) {
                const DAbstractFileInfoPointer &fileInfo = DFileService::instance()->createFileInfo(url);
                if (fileInfo && fileInfo->isDir()) {
                    QProcess::startDetached("gvfs-open", QStringList() << url.toString());
                }
            }
        }
        break;
        case FileManagerProperty: {
            QStringList localFiles;
            for (auto url : this->selectedUrls()) {
                if (isTrashFile(url)) {
                    localFiles << TrashRoot;
                } else {
                    localFiles << url.toLocalFile();
                }
            }
            startProcessDetached("/usr/bin/dde-property-dialog",
                                 localFiles);
        }
        break;
        case MenuAction::Rename: {
            QAbstractItemView::edit(index);
            break;
        }
        }
    });

    menu->exec();
    menu->deleteLater();
}
