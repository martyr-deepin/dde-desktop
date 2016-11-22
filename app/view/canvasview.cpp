#include "canvasview.h"


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

#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QUrlQuery>
#include <QActionGroup>
#include <QContextMenuEvent>
#include <QHeaderView>
#include <QMimeData>

DWIDGET_USE_NAMESPACE

#define ICON_VIEW_SPACING 5
#define LIST_VIEW_SPACING 1


// begin file item global define
#define TEXT_PADDING 5
#define ICON_MODE_ICON_SPACING 5
#define COLUMU_PADDING 10
#define LEFT_PADDING 10
#define RIGHT_PADDING 10
// end

// begin file view global define
#define LIST_MODE_LEFT_MARGIN 20
#define LIST_MODE_RIGHT_MARGIN 20
// end

#define DEFAULT_HEADER_SECTION_WIDTH 140

#include "canvasviewhelper.h"
#include "../model/dfileselectionmodel.h"

class CanvasViewPrivate
{
public:
    CanvasViewPrivate(CanvasView *qq)
        : q_ptr(qq) {}

    CanvasView *q_ptr;

    DFileMenuManager *fileMenuManager;

    QList<int> columnRoles;

    CanvasView::ViewMode defaultViewMode = CanvasView::IconMode;
    CanvasView::ViewMode currentViewMode = CanvasView::IconMode;

    QRect selectedGeometry;
    QWidget *selectionRectWidget = Q_NULLPTR;
    bool selectionRectVisible = true;

    int horizontalOffset    = 0;
    int verticalOffset      = 0;
    QMargins margins        = QMargins(0, 0, 0, 0);

    /// move cursor later selecte index when pressed key shift
    QModelIndex lastCursorIndex;


    DUrlList preSelectionUrls;

    Anchors<QLabel> contentLabel = Q_NULLPTR;

    QModelIndex mouseLastPressedIndex;

    /// drag drop
    QModelIndex dragMoveHoverIndex;

    CanvasViewHelper *fileViewHelper;

    Q_DECLARE_PUBLIC(CanvasView)
};

CanvasView::CanvasView(QWidget *parent)
    : DListView(parent)
    , d_ptr(new CanvasViewPrivate(this))
{
    D_THEME_INIT_WIDGET(CanvasView);
    D_D(CanvasView);

    initUI();
    initDelegate();
    initModel();
    initConnects();
}

CanvasView::~CanvasView()
{
    disconnect(this, &CanvasView::rowCountChanged, this, &CanvasView::onRowCountChanged);

}

void CanvasView::initDelegate()
{
    D_D(CanvasView);

    setItemDelegate(new DIconItemDelegate(d->fileViewHelper));
}

void CanvasView::initUI()
{
    D_D(CanvasView);

    setSpacing(ICON_VIEW_SPACING);
    setResizeMode(QListView::Adjust);
    setOrientation(QListView::LeftToRight, true);
    setTextElideMode(Qt::ElideMiddle);
    setDragDropMode(QAbstractItemView::DragDrop);
    setDropIndicatorShown(false);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setSelectionBehavior(QAbstractItemView::SelectItems);
    setEditTriggers(QListView::EditKeyPressed | QListView::SelectedClicked);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBar(new DScrollBar);

    DListView::setSelectionRectVisible(true);

    d->fileViewHelper = new CanvasViewHelper(this);

    d->selectionRectWidget = new QWidget(this);
    d->selectionRectWidget->hide();
    d->selectionRectWidget->resize(0, 0);
    d->selectionRectWidget->setObjectName("SelectionRect");
    d->selectionRectWidget->raise();
    d->selectionRectWidget->setAttribute(Qt::WA_TransparentForMouseEvents);

    auto margin = ICON_VIEW_SPACING * 2.9;
    d->margins = QMargins(margin, margin, margin, margin);
}

void CanvasView::initModel()
{
    D_D(CanvasView);

    setModel(new DFileSystemModel(d->fileViewHelper));
    setSelectionModel(new DFileSelectionModel(model(), this));
}

void CanvasView::initConnects()
{
    D_D(CanvasView);

    connect(this, &CanvasView::doubleClicked,
    this, [this](const QModelIndex & index) {
        if (!DFMGlobal::keyCtrlIsPressed() && !DFMGlobal::keyShiftIsPressed()) {
            openIndex(index);
        }
    }, Qt::QueuedConnection);

    connect(this, &CanvasView::rowCountChanged, this, &CanvasView::onRowCountChanged, Qt::QueuedConnection);

    connect(model(), &DFileSystemModel::dataChanged, this, &CanvasView::handleDataChanged);
    connect(model(), &DFileSystemModel::stateChanged, this, &CanvasView::onModelStateChanged);
}

DFileSystemModel *CanvasView::model() const
{
    return qobject_cast<DFileSystemModel *>(DListView::model());
}

DStyledItemDelegate *CanvasView::itemDelegate() const
{
    return qobject_cast<DStyledItemDelegate *>(DListView::itemDelegate());
}

void CanvasView::setItemDelegate(DStyledItemDelegate *delegate)
{
    D_D(CanvasView);

    QAbstractItemDelegate *dg = DListView::itemDelegate();

    if (dg) {
        dg->deleteLater();
    }

    DListView::setItemDelegate(delegate);

    connect(delegate, &DStyledItemDelegate::commitData, this, &CanvasView::handleCommitData);
}


CanvasViewHelper *CanvasView::fileViewHelper() const
{
    D_DC(CanvasView);

    return d->fileViewHelper;
}

DUrl CanvasView::currentUrl() const
{
    return model()->getUrlByIndex(rootIndex());
}

QList<DUrl> CanvasView::selectedUrls() const
{
    DUrlList list;

    for (const QModelIndex &index : selectedIndexes()) {
        list << model()->getUrlByIndex(index);
    }

    return list;
}

int CanvasView::rowCount() const
{
    int count = this->count();
    int itemCountForRow = this->itemCountForRow();

    return count / itemCountForRow + int(count % itemCountForRow > 0);
}

int CanvasView::itemCountForRow() const
{

    int itemWidth = itemSizeHint().width() + ICON_VIEW_SPACING * 2;

    return (width() - ICON_VIEW_SPACING * 2.9) / itemWidth;
}

void CanvasView::setIconSize(const QSize &size)
{
    DListView::setIconSize(size);

    updateHorizontalOffset();
    updateGeometries();
}

int CanvasView::getSortRoles() const
{
    return model()->sortRole();
}


int CanvasView::horizontalOffset() const
{
    D_DC(CanvasView);

    return d->horizontalOffset;
}

int CanvasView::verticalOffset() const
{
    D_DC(CanvasView);

    return d->verticalOffset;
}

bool CanvasView::isSelected(const QModelIndex &index) const
{
    return static_cast<DFileSelectionModel *>(selectionModel())->isSelected(index);
}

int CanvasView::selectedIndexCount() const
{
    return static_cast<const DFileSelectionModel *>(selectionModel())->selectedCount();
}

QModelIndexList CanvasView::selectedIndexes() const
{
    return static_cast<DFileSelectionModel *>(selectionModel())->selectedIndexes();
}

QRect CanvasView::visualRect(const QModelIndex &index) const
{
    D_DC(CanvasView);

    QRect rect;

    if (index.column() != 0) {
        qCritical() << "list view colum must zero" << index;
        return rect;
    }

    auto margins = d->margins;
    auto space  = this->spacing();
    QSize item_size = itemSizeHint();

    if (item_size.width() == -1) {
        // use (0,0)
        rect.setTopLeft(QPoint(margins.left(), margins.top()));
        rect.setSize(item_size);
    } else {
        int item_width = item_size.width() + space * 2;
        int item_height = item_size.height() + space * 2;
        int row_count = (height() - margins.top() - margins.bottom()) / item_height;
        int row_index = index.row() % row_count;
        int column_index = index.row() / row_count;

        rect.setTop(row_index * (item_size.height()  + space * 2) + space);
        rect.setLeft(column_index * item_width + space);
        rect.setSize(item_size);
    }

    rect.moveLeft(rect.left() - horizontalOffset());
    rect.moveTop(rect.top() - verticalOffset());

    return rect;
}

QModelIndex CanvasView::indexAt(const QPoint &point) const
{
    for (QModelIndex &index : itemDelegate()->hasWidgetIndexs()) {
        if (index == itemDelegate()->editingIndex()) {
            continue;
        }

        QWidget *widget = indexWidget(index);

        qDebug() << widget->geometry().contains(point);
        if (widget && widget->isVisible() && widget->geometry().contains(point)) {
            return index;
        }
    }


//    FileIconItem *item = qobject_cast<FileIconItem*>(itemDelegate()->editingIndexWidget());

//    if (item) {
//        QRect geometry = item->icon->geometry();

//        geometry.moveTopLeft(geometry.topLeft() + item->pos());

//        if (geometry.contains(point))
//            return itemDelegate()->editingIndex();

//        geometry = item->edit->geometry();
//        geometry.moveTopLeft(geometry.topLeft() + item->pos());
//        geometry.setTop(item->icon->y() + item->icon->height() + item->y());

//        if (geometry.contains(point))
//            return itemDelegate()->editingIndex();
//    }

    D_DC(CanvasView);
    QPoint pos = QPoint(point.x() + horizontalOffset(), point.y() + verticalOffset());

    QSize item_size = itemSizeHint();
    int gridWidth = item_size.width() + spacing() * 2;
    int gridHeight = item_size.height() + spacing() * 2;
    auto margins = d->margins;
    if (pos.y() % gridHeight < spacing()) {
        qDebug() << "return empty";
        return QModelIndex();
    }

    int index = -1;

    if (item_size.width() == -1) {
        qDebug() << "return empty";
        index = pos.y() / gridHeight;
    } else {
        int column_index = pos.x() / gridWidth;
        int row_index = pos.y() / gridHeight;
        int row_count = (height() - margins.top() - margins.bottom()) / gridHeight;

        if (row_index >= row_count) {
            return QModelIndex();
        }

        qDebug() << row_index << column_index;
        index = column_index * row_count + row_index;

        const QModelIndex &tmp_index = rootIndex().child(index, 0);
        QStyleOptionViewItem option = viewOptions();

        option.rect = QRect(QPoint(column_index * gridWidth + ICON_VIEW_SPACING,
                                   row_index * (gridHeight) + ICON_VIEW_SPACING),
                            item_size);

        const QList<QRect> &list = itemDelegate()->paintGeomertys(option, tmp_index);

        qDebug() << tmp_index;
        for (const QRect &rect : list)
            if (rect.contains(pos)) {
                return tmp_index;
            }

        return QModelIndex();
    }
    return rootIndex().child(index, 0);
}

CanvasView::RandeIndexList CanvasView::visibleIndexes(QRect rect) const
{
    qDebug() << rect;
    RandeIndexList list;

    QSize item_size = itemSizeHint();
    QSize icon_size = iconSize();

    int count = this->count();
    int spacing  = this->spacing();
    int item_width = item_size.width() + spacing *  2;
    int item_height = item_size.height() + spacing * 2;

    if (item_size.width() == -1) {
        list << RandeIndex(qMax((rect.top() + spacing) / item_height, 0),
                           qMin((rect.bottom() - spacing) / item_height, count - 1));
    } else {
        rect -= QMargins(spacing, spacing, spacing, spacing);

        int row_count = (width() - spacing * 2.9) / item_height;
        int begin_column_index = rect.left() / item_width;
        int end_column_index = rect.right() / item_width;
        int begin_row_index = rect.top() / item_height;
        int end_row_index = rect.bottom() / item_height;

        if (rect.top() % item_height > icon_size.height()) {
            ++begin_row_index;
        }

        int icon_margin = (item_width - icon_size.width()) / 2;

        if (rect.left() % item_width > item_width - icon_margin) {
            ++begin_column_index;
        }

        if (rect.right() % item_width < icon_margin) {
            --end_column_index;
        }

        begin_row_index = qMax(begin_row_index, 0);
        begin_column_index = qMax(begin_column_index, 0);
        end_row_index = qMin(end_row_index, row_count - 1);
        end_column_index = qMin(end_column_index, count / row_count);

        if (begin_row_index > end_row_index || begin_column_index > end_column_index) {
            return list;
        }

        int begin_index = begin_column_index * row_count;

        for (int i = begin_column_index; i <= end_row_index; ++i) {
            if (begin_index + begin_row_index >= count) {
                break;
            }

            list << RandeIndex(qMax(begin_index + begin_row_index, 0),
                               qMin(begin_index + end_row_index, count - 1));

            begin_index += row_count;
        }
    }

    return list;
}

QSize CanvasView::itemSizeHint() const
{
    D_DC(CanvasView);

    return itemDelegate()->sizeHint(QStyleOptionViewItem(), QModelIndex());
}

bool CanvasView::isDropTarget(const QModelIndex &index) const
{
    D_DC(CanvasView);

    return d->dragMoveHoverIndex == index;
}

bool CanvasView::cd(const DUrl &url)
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

bool CanvasView::cdUp()
{
    const DAbstractFileInfoPointer &fileInfo = model()->fileInfo(rootIndex());

    const DUrl &oldCurrentUrl = currentUrl();
    const DUrl &parentUrl = fileInfo ? fileInfo->parentUrl() : DUrl::parentUrl(oldCurrentUrl);

    return cd(parentUrl);
}

bool CanvasView::edit(const QModelIndex &index, QAbstractItemView::EditTrigger trigger, QEvent *event)
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

    return DListView::edit(index, trigger, event);
}

void CanvasView::select(const QList<DUrl> &list)
{
    QModelIndex firstIndex;
    QModelIndex lastIndex;

    clearSelection();

    for (const DUrl &url : list) {
        const QModelIndex &index = model()->index(url);

        if (index.isValid()) {
            selectionModel()->select(index, QItemSelectionModel::Select);
        }

        if (!firstIndex.isValid()) {
            firstIndex = index;
        }

        lastIndex = index;
    }

    if (lastIndex.isValid()) {
        selectionModel()->setCurrentIndex(lastIndex, QItemSelectionModel::Select);
    }

    if (firstIndex.isValid()) {
        scrollTo(firstIndex, PositionAtTop);
    }
}

void CanvasView::sortByRole(int role)
{
    D_D(CanvasView);

    Qt::SortOrder order = (model()->sortRole() == role && model()->sortOrder() == Qt::AscendingOrder)
                          ? Qt::DescendingOrder
                          : Qt::AscendingOrder;

    model()->setSortRole(role, order);
    model()->sort();
}

QStringList CanvasView::nameFilters() const
{
    return model()->nameFilters();
}

void CanvasView::setNameFilters(const QStringList &nameFilters)
{
    model()->setNameFilters(nameFilters);
}

QDir::Filters CanvasView::filters() const
{
    return model()->filters();
}

void CanvasView::setFilters(QDir::Filters filters)
{
    model()->setFilters(filters);
}

void CanvasView::sortByActionTriggered(QAction *action)
{
    D_DC(CanvasView);

    DAction *dAction = static_cast<DAction *>(action);

//    sortByColumn(d->sortByActionGroup->actions().indexOf(dAction));
}

void CanvasView::openWithActionTriggered(QAction *action)
{
    DAction *dAction = static_cast<DAction *>(action);
    QString app = dAction->property("app").toString();
    QString url = dAction->property("url").toString();
    qDebug() << "Todo: FileUtils::openFileByApp(url, app)";
}

void CanvasView::onRowCountChanged()
{
    static_cast<DFileSelectionModel *>(selectionModel())->m_selectedList.clear();
}

void CanvasView::wheelEvent(QWheelEvent *event)
{
    if (DFMGlobal::keyCtrlIsPressed()) {
        if (event->angleDelta().y() > 0) {
            increaseIcon();
        } else {
            decreaseIcon();
        }

        event->accept();
    } else {
        verticalScrollBar()->setSliderPosition(verticalScrollBar()->sliderPosition() - event->angleDelta().y());
    }
}

void CanvasView::keyPressEvent(QKeyEvent *event)
{
    D_D(CanvasView);

    const DUrlList &urls = selectedUrls();


    switch (event->modifiers()) {
    case Qt::NoModifier:
    case Qt::KeypadModifier:
        switch (event->key()) {
        case Qt::Key_Return:
        case Qt::Key_Enter:
            if (!itemDelegate()->editingIndex().isValid()) {

                return;
            }

            break;
        case Qt::Key_Backspace:
            cdUp();

            return;
        case Qt::Key_F1:

            return;
        case Qt::Key_F5:
            model()->refresh();

            return;
        case Qt::Key_Delete:

            break;
        default: break;
        }

        break;
    case Qt::ControlModifier:
        switch (event->key()) {
        case Qt::Key_F:
            return;
        case Qt::Key_L:

            return;
        case Qt::Key_N:

            return;
        case Qt::Key_H:
            d->preSelectionUrls = urls;

            itemDelegate()->hideAllIIndexWidget();
            clearSelection();


            return;
        case Qt::Key_I:

            return;
        case Qt::Key_Up:
            cdUp();

            return;
        case Qt::Key_Down:
            return;
        case Qt::Key_Left:

            return;
        case Qt::Key_Right:

            return;
        case Qt::Key_T:
            return;
        case Qt::Key_W:
            return;
        default: break;
        }

        break;
    case Qt::ShiftModifier:
        if (event->key() == Qt::Key_Delete) {

            return;
        } else if (event->key() == Qt::Key_T) {

            return;
        }

        break;
    case Qt::ControlModifier | Qt::ShiftModifier:
        if (event->key() == Qt::Key_N) {
            if (itemDelegate()->editingIndex().isValid()) {
                return;
            }

            clearSelection();

            return;
        } if (event->key() == Qt::Key_Question) {

            return;
        }

        break;
    case Qt::AltModifier:
        switch (event->key()) {
        case Qt::Key_Up:
            cdUp();

            return;
        case Qt::Key_Down:

            return;
        case Qt::Key_Left:

            return;
        case Qt::Key_Right:

            return;
        case Qt::Key_Home:
            cd(DUrl::fromLocalFile(QDir::homePath()));

            return;
        default: break;
        }

        break;
    default: break;
    }

    DListView::keyPressEvent(event);
}

void CanvasView::showEvent(QShowEvent *event)
{
    DListView::showEvent(event);

    setFocus();
}

void CanvasView::mousePressEvent(QMouseEvent *event)
{
    D_D(CanvasView);

    switch (event->button()) {
    case Qt::BackButton: {

        break;
    }
    case Qt::ForwardButton: {


        break;
    }
    case Qt::LeftButton: {
        bool isEmptyArea = d->fileViewHelper->isEmptyArea(event->pos());

        setDragEnabled(!isEmptyArea);

        if (isEmptyArea) {
            if (!DFMGlobal::keyCtrlIsPressed()) {
                itemDelegate()->hideNotEditingIndexWidget();
                clearSelection();
            }

            if (canShowSelectionRect()) {
                d->selectionRectWidget->show();
            }

            d->selectedGeometry.setTop(event->pos().y() + verticalOffset());
            d->selectedGeometry.setLeft(event->pos().x() + horizontalOffset());

            connect(verticalScrollBar(), &QScrollBar::valueChanged, this, &CanvasView::updateSelectionRect);
        } else if (DFMGlobal::keyCtrlIsPressed()) {
            const QModelIndex &index = indexAt(event->pos());

            if (selectionModel()->isSelected(index)) {
                d->mouseLastPressedIndex = index;

                DListView::mousePressEvent(event);

                selectionModel()->select(index, QItemSelectionModel::Select);

                return;
            }
        }

        d->mouseLastPressedIndex = QModelIndex();

        DListView::mousePressEvent(event);
        break;
    }
    default: break;
    }
}

void CanvasView::mouseMoveEvent(QMouseEvent *event)
{
    if (dragEnabled()) {
        return DListView::mouseMoveEvent(event);
    }

    updateSelectionRect();
    doAutoScroll();
}

void CanvasView::mouseReleaseEvent(QMouseEvent *event)
{
    D_D(CanvasView);

    d->dragMoveHoverIndex = QModelIndex();

    disconnect(verticalScrollBar(), &QScrollBar::valueChanged, this, &CanvasView::updateSelectionRect);

    if (d->mouseLastPressedIndex.isValid() && DFMGlobal::keyCtrlIsPressed()) {
        if (d->mouseLastPressedIndex == indexAt(event->pos())) {
            selectionModel()->select(d->mouseLastPressedIndex, QItemSelectionModel::Deselect);
        }
    }

    if (dragEnabled()) {
        return DListView::mouseReleaseEvent(event);
    }

    d->selectionRectWidget->resize(0, 0);
    d->selectionRectWidget->hide();
}

void CanvasView::handleCommitData(QWidget *editor)
{
    if (!editor) {
        return;
    }

    const DAbstractFileInfoPointer &fileInfo = model()->fileInfo(itemDelegate()->editingIndex());

    if (!fileInfo) {
        return;
    }

//    QLineEdit *lineEdit = qobject_cast<QLineEdit *>(editor);
//    FileIconItem *item = qobject_cast<FileIconItem *>(editor);

//    QString new_file_name = lineEdit ? lineEdit->text() : item ? item->edit->toPlainText() : "";

//    new_file_name.remove('/');
//    new_file_name.remove(QChar(0));

//    if (fileInfo->fileName() == new_file_name || new_file_name.isEmpty()) {
//        return;
//    }

//    DUrl old_url = fileInfo->fileUrl();
//    DUrl new_url = fileInfo->getUrlByNewFileName(new_file_name);

//    if (lineEdit) {
//        /// later rename file.

//    } else {
//    }
}


void CanvasView::focusInEvent(QFocusEvent *event)
{
    DListView::focusInEvent(event);
    itemDelegate()->commitDataAndCloseActiveEditor();

    const DUrl &current_url = currentUrl();

    if (current_url.isLocalFile()) {
        QDir::setCurrent(current_url.toLocalFile());
    }
}

void CanvasView::resizeEvent(QResizeEvent *event)
{
    DListView::resizeEvent(event);

    updateHorizontalOffset();

    if (itemDelegate()->editingIndex().isValid()) {
        doItemsLayout();
    }
}

void CanvasView::contextMenuEvent(QContextMenuEvent *event)
{
    D_DC(CanvasView);

    bool isEmptyArea = d->fileViewHelper->isEmptyArea(event->pos());

    const QModelIndex &index = indexAt(event->pos());

    if (isEmptyArea  && !selectionModel()->isSelected(index)) {
        itemDelegate()->hideNotEditingIndexWidget();
        clearSelection();
        showEmptyAreaMenu();
    } else {
        if (!isSelected(index)) {
            setCurrentIndex(index);
        }

        showNormalMenu(index);
    }
}

void CanvasView::dragEnterEvent(QDragEnterEvent *event)
{
    for (const DUrl &url : event->mimeData()->urls()) {
        const DAbstractFileInfoPointer &fileInfo = DFileService::instance()->createFileInfo(url);

        if (!fileInfo->isWritable()) {
            event->ignore();

            return;
        }
    }

    preproccessDropEvent(event);

    if (event->mimeData()->hasFormat("XdndDirectSave0")) {
        event->setDropAction(Qt::CopyAction);
        event->acceptProposedAction();

        return;
    }

    DListView::dragEnterEvent(event);
}

void CanvasView::dragMoveEvent(QDragMoveEvent *event)
{
    D_D(CanvasView);

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

void CanvasView::dragLeaveEvent(QDragLeaveEvent *event)
{
    D_D(CanvasView);

    d->dragMoveHoverIndex = QModelIndex();

    DListView::dragLeaveEvent(event);
}

void CanvasView::dropEvent(QDropEvent *event)
{
    D_D(CanvasView);

    d->dragMoveHoverIndex = QModelIndex();

    preproccessDropEvent(event);

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

        if (isSelected(index)) {
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

void CanvasView::setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags flags)
{
    qDebug() << rect;
    if (DFMGlobal::keyShiftIsPressed()) {
        const QModelIndex &index = indexAt(rect.bottomRight());

        if (!index.isValid()) {
            return;
        }

        const QModelIndex &lastSelectedIndex = indexAt(rect.topLeft());

        if (!lastSelectedIndex.isValid()) {
            return;
        }

        selectionModel()->select(QItemSelection(lastSelectedIndex, index), QItemSelectionModel::ClearAndSelect);

        return;
    }

    qDebug() << flags;
    if (flags & QItemSelectionModel::ClearAndSelect) {
        QRect tmp_rect;

        tmp_rect.setCoords(qMin(rect.left(), rect.right()), qMin(rect.top(), rect.bottom()),
                           qMax(rect.left(), rect.right()), qMax(rect.top(), rect.bottom()));

        const RandeIndexList &list = visibleIndexes(tmp_rect);

        if (list.isEmpty()) {
            clearSelection();
            return;
        }

        return selectionModel()->select(QItemSelection(rootIndex().child(list.first().first, 0),
                                        rootIndex().child(list.last().second, 0)), flags);
    }

    DListView::setSelection(rect, flags);
}

QModelIndex CanvasView::moveCursor(QAbstractItemView::CursorAction cursorAction, Qt::KeyboardModifiers modifiers)
{
    D_D(CanvasView);

    QModelIndex current = currentIndex();

    if (!current.isValid()) {
        d->lastCursorIndex = DListView::moveCursor(cursorAction, modifiers);

        return d->lastCursorIndex;
    }

    if (rectForIndex(current).isEmpty()) {
        d->lastCursorIndex = model()->index(0, 0, rootIndex());

        return d->lastCursorIndex;
    }

    QModelIndex index;

    switch (cursorAction) {
    case MoveLeft:
        if (DFMGlobal::keyShiftIsPressed()) {
            index = DListView::moveCursor(cursorAction, modifiers);

            if (index == d->lastCursorIndex) {
                index = index.sibling(index.row() - 1, index.column());
            }
        } else {
            index = current.sibling(current.row() - 1, current.column());
        }

        break;
    case MoveRight:
        if (DFMGlobal::keyShiftIsPressed()) {
            index = DListView::moveCursor(cursorAction, modifiers);

            if (index == d->lastCursorIndex) {
                index = index.sibling(index.row() + 1, index.column());
            }
        } else {
            index = current.sibling(current.row() + 1, current.column());
        }

        break;
    default:
        index = DListView::moveCursor(cursorAction, modifiers);
        break;
    }

    if (index.isValid()) {
        d->lastCursorIndex = index;

        return index;
    }

    d->lastCursorIndex = current;

    return current;
}

void CanvasView::rowsAboutToBeRemoved(const QModelIndex &parent, int start, int end)
{
    for (const QModelIndex &index : selectedIndexes()) {
        if (index.parent() != parent) {
            continue;
        }

        if (index.row() >= start && index.row() <= end) {
            selectionModel()->select(index, QItemSelectionModel::Clear);
        }
    }

    DListView::rowsAboutToBeRemoved(parent, start, end);
}

void CanvasView::rowsInserted(const QModelIndex &parent, int start, int end)
{
    DListView::rowsInserted(parent, start, end);
}

void CanvasView::increaseIcon()
{
    D_D(CanvasView);

    int iconSizeLevel = itemDelegate()->increaseIcon();
}

void CanvasView::decreaseIcon()
{
    D_D(CanvasView);

    int iconSizeLevel = itemDelegate()->decreaseIcon();
}

void CanvasView::openIndex(const QModelIndex &index)
{
    D_D(CanvasView);

    if (model()->hasChildren(index)) {
        DUrl url = model()->getUrlByIndex(index);
        if (url.isUserShareFile()) {
            url.setScheme(FILE_SCHEME);
        }

//        d->fileViewHelper->preHandleCd(event);
    } else {
//        emit fileService->openFile(model()->getUrlByIndex(index));
    }
}

void CanvasView::keyboardSearch(const QString &search)
{
    D_D(CanvasView);

    if (search.isEmpty()) {
        return;
    }

    d->fileViewHelper->keyboardSearch(search.toLocal8Bit().at(0));
}

bool CanvasView::setCurrentUrl(const DUrl &url)
{
    D_D(CanvasView);

    DUrl fileUrl = url;

    if (fileUrl.isTrashFile() && fileUrl.path().isEmpty()) {
        fileUrl.setPath("/");
    }

    const DAbstractFileInfoPointer &info = DFileService::instance()->createFileInfo(fileUrl);

    if (!info) {
        qDebug() << "This scheme isn't support";
        return false;
    }

    if (info->canRedirectionFileUrl()) {
        const DUrl old_url = fileUrl;

        fileUrl = info->redirectedFileUrl();

        qDebug() << "url redirected, from:" << old_url << "to:" << fileUrl;
    }

    qDebug() << "cd: current url:" << currentUrl() << "to url:" << fileUrl;

    const DUrl &currentUrl = this->currentUrl();


    if (currentUrl == fileUrl/* && !info->isShared()*/) {
        return false;
    }

//    QModelIndex index = model()->index(fileUrl);

//    if(!index.isValid())
    QModelIndex index = model()->setRootUrl(fileUrl);

//    model()->setActiveIndex(index);
    setRootIndex(index);

    if (!model()->canFetchMore(index)) {
        qDebug() << "updateContentLabel()";
    }
//        model()->setSortRole(sort_config.first, (Qt::SortOrder)sort_config.second);


    if (info) {
        switchViewMode(IconMode);
    }
    emit currentUrlChanged(fileUrl);

    if (focusWidget() && focusWidget()->window() == window() && fileUrl.isLocalFile()) {
        QDir::setCurrent(fileUrl.toLocalFile());
    }

//    setSelectionMode(info->supportSelectionMode());

    const DUrl &defaultSelectUrl = DUrl(QUrlQuery(fileUrl.query()).queryItemValue("selectUrl"));

    if (defaultSelectUrl.isValid()) {
        d->preSelectionUrls << defaultSelectUrl;
    } else {
        DUrl oldCurrentUrl = currentUrl;

        forever {
            const DUrl &tmp_url = oldCurrentUrl.parentUrl();

            if (tmp_url == fileUrl || !tmp_url.isValid())
            {
                break;
            }

            oldCurrentUrl = tmp_url;
        }

        d->preSelectionUrls << oldCurrentUrl;
    }

    return true;
}

void CanvasView::clearSelection()
{
    QListView::clearSelection();
}

void CanvasView::handleDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
{
    DListView::dataChanged(topLeft, bottomRight, roles);

    for (int i = topLeft.row(); i <= bottomRight.row(); ++i) {
        update(model()->index(i, 0));
    }
}


void CanvasView::setSelectionRectVisible(bool visible)
{
    D_D(CanvasView);

    d->selectionRectVisible = visible;
}

bool CanvasView::isSelectionRectVisible() const
{
    D_DC(CanvasView);

    return d->selectionRectVisible;
}

bool CanvasView::canShowSelectionRect() const
{
    D_DC(CanvasView);

    return d->selectionRectVisible && selectionMode() != NoSelection && selectionMode() != SingleSelection;
}

void CanvasView::updateHorizontalOffset()
{
    D_D(CanvasView);

    int contentWidth = width();
    int itemWidth = itemSizeHint().width() + ICON_VIEW_SPACING * 2;
    int itemColumn = (contentWidth - ICON_VIEW_SPACING * 2.9) / itemWidth;

    d->horizontalOffset = -(contentWidth - itemWidth * itemColumn) / 2;
}

void CanvasView::switchViewMode(CanvasView::ViewMode mode)
{
    D_D(CanvasView);

    if (d->currentViewMode == mode) {
        return;
    }

    const DAbstractFileInfoPointer &fileInfo = model()->fileInfo(currentUrl());

    if (fileInfo && (fileInfo->supportViewMode() & mode) == 0) {
        return;
    }

    d->currentViewMode = mode;

    itemDelegate()->hideAllIIndexWidget();

    switch (mode) {
    case IconMode: {
        d->columnRoles.clear();
        setOrientation(QListView::LeftToRight, true);
        setSpacing(ICON_VIEW_SPACING);
        setItemDelegate(new DIconItemDelegate(d->fileViewHelper));

        break;
    }
    default:
        break;
    }

    setFocus();
}

void CanvasView::showEmptyAreaMenu()
{
//    D_D(CanvasView);

    const QModelIndex &index = rootIndex();
    const DAbstractFileInfoPointer &info = model()->fileInfo(index);
    QVector<MenuAction> actions = info->menuActionList(DAbstractFileInfo::SpaceArea);

    // use white list
    actions.remove(actions.indexOf(MenuAction::DisplayAs));

    if (actions.isEmpty()) {
        return;
    }

    const QMap<MenuAction, QVector<MenuAction> > &subActions = info->subMenuActionList();

    QSet<MenuAction> disableList = DFileMenuManager::getDisableActionList(model()->getUrlByIndex(index));

    if (model()->state() != DFileSystemModel::Idle) {
        disableList << MenuAction::SortBy;
    }

    if (!count()) {
        disableList << MenuAction::SelectAll;
    }

    disableList << MenuAction::DisplayAs;
    qDebug() << subActions.keys();

    DFileMenu *menu = DFileMenuManager::genereteMenuByKeys(actions, disableList, true, subActions);
    DAction *tmp_action = menu->actionAt(fileMenuManger->getActionString(MenuAction::SortBy));
    DFileMenu *sortBySubMenu = static_cast<DFileMenu *>(tmp_action ? tmp_action->menu() : Q_NULLPTR);
    if (sortBySubMenu) {
        DAction *action = sortBySubMenu->actionAt(model()->sortColumn());
        if (action) {
            action->setChecked(true);
        }
    }
    auto  rootUrl = model()->getUrlByIndex(rootIndex());

    DUrlList urls;
    urls.append(rootUrl);
    DFMEvent event;
    event << rootUrl;
    event << urls;
    event << this->winId();
    event << DFMEvent::FileView;
    menu->setEvent(event);

    connect(menu, &DFileMenu::triggered, this, [ = ](DAction * action) {
        qDebug() << action->data();
        if (action->data().isValid()) {
            bool flag = false;
            int _type = action->data().toInt(&flag);
            MenuAction type;
            if (flag) {
                type = (MenuAction)_type;
            } else {
                qDebug() << action->data().toString();;
                return;
            }

            QMetaEnum metaEnum = QMetaEnum::fromType<MenuAction>();
            QString key = metaEnum.valueToKey(type);
            qDebug() << key;
        }
    });
    menu->exec();
    menu->deleteLater();
}


void CanvasView::showNormalMenu(const QModelIndex &index)
{
    D_D(CanvasView);

    if (!index.isValid()) {
        return;
    }

    DUrlList list = selectedUrls();

    DFileMenu *menu;

    const DAbstractFileInfoPointer &info = model()->fileInfo(index);

    if (list.length() == 1) {
        const QVector<MenuAction> &actions = info->menuActionList(DAbstractFileInfo::SingleFile);

        if (actions.isEmpty()) {
            return;
        }

        const QMap<MenuAction, QVector<MenuAction> > &subActions = info->subMenuActionList();
        QSet<MenuAction> disableList = DFileMenuManager::getDisableActionList(list);

        menu = DFileMenuManager::genereteMenuByKeys(actions, disableList, true, subActions);

        DAction *openWithAction = menu->actionAt(DFileMenuManager::getActionString(DFMGlobal::OpenWith));
        DFileMenu *openWithMenu = openWithAction ? qobject_cast<DFileMenu *>(openWithAction->menu()) : Q_NULLPTR;

        if (openWithMenu) {
            QMimeType mimeType = info->mimeType();
            QStringList recommendApps;


            foreach(QString app, recommendApps) {
                DAction *action = new DAction("LoaclName", 0);
                action->setProperty("app", app);
                action->setProperty("url", info->redirectedFileUrl().toLocalFile());
                openWithMenu->addAction(action);
            }

            DAction *action = new DAction(fileMenuManger->getActionString(MenuAction::OpenWithCustom), 0);
            action->setData((int)MenuAction::OpenWithCustom);
            openWithMenu->addAction(action);
        }
    } else {
        bool isSystemPathIncluded = false;
        bool isAllCompressedFiles = true;

        foreach(DUrl url, list) {
            const DAbstractFileInfoPointer &fileInfo = fileService->createFileInfo(url);
            if (fileInfo->isActive()) {
                isAllCompressedFiles = false;
            }
        }

        QVector<MenuAction> actions;

        if (isSystemPathIncluded) {
            actions = info->menuActionList(DAbstractFileInfo::MultiFilesSystemPathIncluded);
        } else {
            actions = info->menuActionList(DAbstractFileInfo::MultiFiles);
        }

        if (actions.isEmpty()) {
            return;
        }

        if (isAllCompressedFiles) {
            int index = actions.indexOf(MenuAction::Compress);
            actions.insert(index + 1, MenuAction::Decompress);
            actions.insert(index + 2, MenuAction::DecompressHere);
        }

        const QMap<MenuAction, QVector<MenuAction> > subActions;
        QSet<MenuAction> disableList = DFileMenuManager::getDisableActionList(list);

        menu = DFileMenuManager::genereteMenuByKeys(actions, disableList, true, subActions);
    }


    menu->exec();
    menu->deleteLater();
}

void CanvasView::onModelStateChanged(int state)
{
    D_D(CanvasView);

    if (state == DFileSystemModel::Busy) {

        qWarning() << "state busy";

    } else if (state == DFileSystemModel::Idle) {
        for (const DUrl &url : d->preSelectionUrls) {
            selectionModel()->select(model()->index(url), QItemSelectionModel::SelectCurrent);
        }

        if (!d->preSelectionUrls.isEmpty()) {
            scrollTo(model()->index(d->preSelectionUrls.first()), PositionAtTop);
        }

        d->preSelectionUrls.clear();


    }
}

void CanvasView::updateSelectionRect()
{
    D_D(CanvasView);

    if (dragEnabled()) {
        return;
    }

    QPoint pos = mapFromGlobal(QCursor::pos());

    if (d->selectionRectWidget->isVisible()) {
        QRect rect;
        QPoint pressedPos = viewport()->mapToParent(d->selectedGeometry.topLeft());

        pressedPos.setX(pressedPos.x() - horizontalOffset());
        pressedPos.setY(pressedPos.y() - verticalOffset());

        rect.setCoords(qMin(pressedPos.x(), pos.x()), qMin(pressedPos.y(), pos.y()),
                       qMax(pos.x(), pressedPos.x()), qMax(pos.y(), pressedPos.y()));

        d->selectionRectWidget->setGeometry(rect);
    }

    pos = viewport()->mapFromParent(pos);

    d->selectedGeometry.setBottom(pos.y() + verticalOffset());
    d->selectedGeometry.setRight(pos.x() + horizontalOffset());

    setSelection(d->selectedGeometry, QItemSelectionModel::Current | QItemSelectionModel::Rows | QItemSelectionModel::ClearAndSelect);
}

void CanvasView::preproccessDropEvent(QDropEvent *event) const
{
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
                if (event->possibleActions().testFlag(action) && info->supportedDropActions().testFlag(action)) {
                    event->setDropAction(action);

                    break;
                }
            }
        }
    }
}
