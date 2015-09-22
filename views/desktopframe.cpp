#include "views/desktopframe.h"
#include "widgets/util.h"
#include "widgets/growingelidetextedit.h"
#include "desktopitemmanager.h"
#include "keyeventmanager.h"
#include "dragdropeventmanager.h"
#include "global.h"

#include <QTimer>


DesktopFrame::DesktopFrame(QWidget *parent)
    : QFrame(parent)
{
    setObjectName("DesktopFrame");
    setAcceptDrops(true);
    setFocus();

    QSettings settings;
    settings.beginGroup("Desktop");
    bool isGridOn = settings.value("isGridOn", true).toBool();
    int sizeType = settings.value("sizeType", SizeType::Middle).toInt();
    settings.endGroup();

    m_isGridOn = isGridOn;
    m_ctrlPressed = false;
    m_isDragStarted = false;
    m_sizeType = static_cast<SizeType>(sizeType);
    m_desktopItemManager = QSharedPointer<DesktopItemManager>::create(this);
    m_keyEventManager = QSharedPointer<KeyEventManager>::create(this);
    m_dragDropEventManager = QSharedPointer<DragDropEventManager>::create(this);

    m_TopDesktopItem = DesktopItemPointer();
    m_lastPressedCheckDesktopItem = DesktopItemPointer();
    m_lastCheckedDesktopItem = DesktopItemPointer();

    m_mouseMoveCheckTimer = new QTimer;
    m_mouseMoveCheckTimer->setSingleShot(true);
    m_mouseMoveCheckTimer->setInterval(10);

    initItems();
    initConnect();

    setGeometry(qApp->desktop()->screenGeometry());
    m_desktopItemManager->checkPageCount();
    qApp->setStyleSheet(getQssFromFile(":/qss/skin/qss/DesktopItem.qss"));
}

void DesktopFrame::initItems(){
    m_gridItems = gridManager->getItemsByType(m_sizeType);
    m_mapItems = gridManager->getMapItems();
    m_desktopItemManager->loadComputerTrashItems();
}

void DesktopFrame::initConnect(){
    connect(signalManager, SIGNAL(pageCountChanged(int)),
            this, SLOT(resizeByPageCount(int)));
    connect(signalManager, SIGNAL(gridSizeTypeChanged(SizeType)),
            this, SLOT(changeGridBySizeType(SizeType)));
    connect(signalManager, SIGNAL(gridModeChanged(bool)),
            this, SLOT(changeGridMode(bool)));
    connect(signalManager, SIGNAL(keyCtrlAPressed()), this, SLOT(handleKeyCtrlAPressed()));
    connect(signalManager, SIGNAL(keyCtrlCPressed()), this, SLOT(handleKeyCtrlCPressed()));
    connect(signalManager, SIGNAL(keyCtrlVPressed()), this, SLOT(handleKeyCtrlVPressed()));
    connect(this, SIGNAL(lastCheckedDesktopItemChanged(DesktopItemPointer)),
            this, SLOT(setLastCheckedDesktopItem(DesktopItemPointer)));
    connect(this, SIGNAL(checkedDesktopItemsAdded(DesktopItemPointer)),
            this, SLOT(addCheckedDesktopItem(DesktopItemPointer)));
    connect(this, SIGNAL(checkedDesktopItemsRemoved(DesktopItemPointer)),
            this, SLOT(removeCheckedDesktopItem(DesktopItemPointer)));
    connect(this, SIGNAL(multiCheckedByMouseChanged(bool)), this, SLOT(setMultiCheckedByMouse(bool)));

    connect(m_mouseMoveCheckTimer, SIGNAL(timeout()), this, SLOT(handleMouseMoveCheckItems()));
}

QSharedPointer<DesktopItemManager> DesktopFrame::getDesktopItemManager(){
    return m_desktopItemManager;
}

SizeType DesktopFrame::getSizeType(){
    return m_sizeType;
}

bool DesktopFrame::isGridOn(){
    return m_isGridOn;
}

bool DesktopFrame::isCtrlPressed(){
    return m_ctrlPressed;
}

void DesktopFrame::setCtrlPressed(bool pressed){
    m_ctrlPressed = pressed;
}

void DesktopFrame::changeGridBySizeType(SizeType type){
    m_gridItems = gridManager->getItemsByType(type);
    m_mapItems = gridManager->getMapItems();
    m_sizeType = type;
    m_desktopItemManager->changeSizeByGrid(type);
    update();
}

void DesktopFrame::changeGridMode(bool mode){
    m_isGridOn = mode;
    if(m_isGridOn){
        gridManager->clearDeskopItemsStatus();
        emit signalManager->gridOnResorted();
    }
}

void DesktopFrame::resizeByPageCount(int pageCount){
    const QRect availableGeometry = QApplication::desktop()->availableGeometry();
    int desktopWidth = availableGeometry.width();
    int desktopHeight = availableGeometry.height();
    QRect r(0, 0, pageCount  * desktopWidth, desktopHeight);
    setGeometry(r);
    update();
}

DesktopItemPointer DesktopFrame::getTopDesktopItemByPos(QPoint pos){
    for (int i=m_desktopItemManager->getItems().count() - 1; i >= 0; i--){
        if (m_desktopItemManager->getItems().at(i)->geometry().contains(pos)){
            m_TopDesktopItem = m_desktopItemManager->getItems().at(i);
            return m_TopDesktopItem;
        }
    }
    return DesktopItemPointer();
}

void DesktopFrame::checkDesktopItemsByRect(QRect rect){
    foreach (DesktopItemPointer pItem, m_desktopItemManager->getItems()) {
        if (rect.intersects(pItem->geometry())){
            if (!pItem->isChecked()){
                pItem->setChecked(true);
                emit lastCheckedDesktopItemChanged(pItem);
                addCheckedDesktopItem(pItem);
            }
        }else{
            if (pItem->isChecked()){
                pItem->setChecked(false);
                if (m_checkedDesktopItems.contains(pItem)){
                    int index = m_checkedDesktopItems.indexOf(pItem);
                    m_checkedDesktopItems.removeAt(index);
                }
            }
        }
    }

    if (m_checkedDesktopItems.count() > 1){
        m_multiCheckedByMouse = true;
    }
}

bool DesktopFrame::isMultiCheckedByMouse(){
    return m_multiCheckedByMouse;
}


void DesktopFrame::setMultiCheckedByMouse(bool flag){
    m_multiCheckedByMouse = flag;
}

bool DesktopFrame::isAllAppCheckedItems(){
    bool flag = true;
    foreach (DesktopItemPointer pItem, m_checkedDesktopItems) {
        flag = flag && isApp(pItem->getUrl()) && pItem->isChecked();
    }
    return flag;
}

void DesktopFrame::unCheckAllItems(){
    foreach (DesktopItemPointer pItem, m_desktopItemManager->getItems()) {
        pItem->setChecked(false);
    }
}

void DesktopFrame::checkAllDesktopItems(){
    foreach (DesktopItemPointer pItem, m_desktopItemManager->getItems()) {
        if (!pItem->isChecked()){
            pItem->setChecked(true);
            emit checkedDesktopItemsAdded(pItem);
        }
    }
}

void DesktopFrame::unCheckCheckedItems(){
    QList<DesktopItemPointer> pItems;
    foreach (DesktopItemPointer pItem, m_checkedDesktopItems) {
        if (pItem->isEditing()){
            pItem->setChecked(true);
            pItem->getTextEdit()->tryRenamed();
            pItem->setEdited(false);
            pItems.append(pItem);
        }else{
            pItem->setChecked(false);
        }
    }
    m_checkedDesktopItems.clear();
    m_checkedDesktopItems.append(pItems);
}


void DesktopFrame::unCheckItem(DesktopItemPointer &pItem){
    if (!pItem.isNull()){
        if (pItem->isChecked()){
            emit pItem->setChecked(false);
        }
        if (m_checkedDesktopItems.contains(pItem)){
            m_checkedDesktopItems.removeOne(pItem);
        }
    }
}

void DesktopFrame::checkRaiseItem(DesktopItemPointer& pItem){
    if (!pItem.isNull()){
        emit lastCheckedDesktopItemChanged(pItem);
        m_multiCheckedByMouse = false;
        pItem->raise();

        bool flag = m_desktopItemManager->getItems().removeOne(pItem);
        if (flag){
            m_desktopItemManager->getItems().append(pItem);
        }
    }
}

DesktopItemPointer DesktopFrame::getLastPressedCheckedDesktopItem(){
    return m_lastPressedCheckDesktopItem;
}

void DesktopFrame::setLastPressedCheckedDesktopItem(DesktopItemPointer pItem){
    m_lastPressedCheckDesktopItem = pItem;
    emit lastCheckedDesktopItemChanged(pItem);
}

DesktopItemPointer DesktopFrame::getLastCheckedDesktopItem(){
    return m_lastCheckedDesktopItem;
}

void DesktopFrame::setLastCheckedDesktopItem(DesktopItemPointer pItem){
    if (!pItem.isNull()){
        pItem->setChecked(true, true);
        m_lastCheckedDesktopItem = pItem;
        addCheckedDesktopItem(pItem);
    }
}

void DesktopFrame::addCheckedDesktopItem(DesktopItemPointer pItem){
    if (!m_checkedDesktopItems.contains(pItem) && !pItem.isNull()){
        m_checkedDesktopItems.append(pItem);
    }
}

void DesktopFrame::removeCheckedDesktopItem(DesktopItemPointer pItem){
    if (m_checkedDesktopItems.contains(pItem)){
        int index = m_checkedDesktopItems.indexOf(pItem);
        m_checkedDesktopItems.removeAt(index);
    }
}

QList<DesktopItemPointer> DesktopFrame::getCheckedDesktopItems(){
    return m_checkedDesktopItems;
}

QPoint DesktopFrame::getAppGroupDestinationPos(){
    return m_appGroupDestinationPos;
}

void DesktopFrame::setAppGroupDestinationPos(QPoint pos){
    m_appGroupDestinationPos = pos;
}

QStringList DesktopFrame::getCheckedFiles(){
    QStringList files;
    foreach (DesktopItemPointer pItem, m_checkedDesktopItems) {
        files.append(pItem->getUrl());
    }
    return files;
}


void DesktopFrame::handleKeyCtrlAPressed(){
    checkAllDesktopItems();
}

void DesktopFrame::handleKeyCtrlCPressed(){
    emit signalManager->filesCopyed(getCheckedFiles());
}

void DesktopFrame::handleKeyCtrlVPressed(){
    emit signalManager->pasteFilesToDesktop();
}

void DesktopFrame::focusInEvent(QFocusEvent *event){
    m_isSelectable = true;
    m_selectRect = QRect(0, 0, 0, 0);
    QFrame::focusInEvent(event);
}


void DesktopFrame::focusOutEvent(QFocusEvent *event){
    m_isSelectable = false;
    m_selectRect = QRect(0, 0, 0, 0);
    QFrame::focusOutEvent(event);
}


void DesktopFrame::dragEnterEvent(QDragEnterEvent *event){
    m_dragLeave = false;

    if (event->source() == this){
        LOG_INFO() << "dragEnterEvent event come from self"<< event->pos();
        event->setDropAction(Qt::MoveAction);
        event->accept();
    }else{
        LOG_INFO() << "dragEnterEvent event come from outside"<< event->pos();
        event->acceptProposedAction();
    }
}


void DesktopFrame::dragMoveEvent(QDragMoveEvent *event){
    m_dragMovePos = event->pos();
    m_dragDropEventManager->handleDragMoveEvent(m_desktopItemManager->getItems(), event);
}

void DesktopFrame::dragLeaveEvent(QDragLeaveEvent *event){
    m_dragLeave = true;
    event->accept();
    LOG_INFO() << "dragLeaveEvent come from outside";
}

void DesktopFrame::dropEvent(QDropEvent *event){

    m_dragDropEventManager->handleDropEvent(m_desktopItemManager->getItems(), event);
}

void DesktopFrame::mousePressEvent(QMouseEvent *event){
    m_isDragStarted = false;
    m_pressedEventPos = event->pos();
    DesktopItemPointer pTopDesktopItem  = getTopDesktopItemByPos(m_pressedEventPos);
    if (event->button() == Qt::LeftButton){
        if (pTopDesktopItem.isNull()){
            if(!m_ctrlPressed){
                setFocus();
                unCheckCheckedItems();
            }
        }else{
            if (m_ctrlPressed){
                if (!pTopDesktopItem->isChecked()){
                    checkRaiseItem(pTopDesktopItem);
                    setLastPressedCheckedDesktopItem(pTopDesktopItem);
                }else{
                    unCheckItem(pTopDesktopItem);
                }
            }else{
                if (!pTopDesktopItem->isChecked()){
                    unCheckCheckedItems();
                    checkRaiseItem(pTopDesktopItem);
                    setLastPressedCheckedDesktopItem(pTopDesktopItem);
                }
            }
            m_isDragStarted = true;
        }

    }else if (event->button() == Qt::RightButton){
        if (pTopDesktopItem.isNull()){
            unCheckCheckedItems();
            emit signalManager->contextMenuShowed(pTopDesktopItem, mapToGlobal(m_pressedEventPos));
        }else{
            if (m_checkedDesktopItems.contains(pTopDesktopItem) && m_checkedDesktopItems.count() > 1){
                emit signalManager->contextMenuShowed(m_checkedDesktopItems, pTopDesktopItem, mapToGlobal(m_pressedEventPos));
            }else{
                unCheckCheckedItems();
                checkRaiseItem(pTopDesktopItem);
                setLastPressedCheckedDesktopItem(pTopDesktopItem);
                emit signalManager->contextMenuShowed(pTopDesktopItem, mapToGlobal(m_pressedEventPos));
                if(isAppGroup(pTopDesktopItem->getUrl())){
                    emit signalManager->appGounpDetailClosed();
                }
            }
        }

    }
    QFrame::mousePressEvent(event);
}

void DesktopFrame::startDrag(){
    QMimeData* mimeData = new QMimeData;
    QList<QUrl> urls;
    foreach (DesktopItemPointer pCheckedItem, m_checkedDesktopItems) {
        urls.append(QUrl(pCheckedItem->getDesktopItemInfo().URI));
    }
    mimeData->setUrls(urls);
    QPixmap dragPixmap = getCheckedPixmap();

    if (m_checkedDesktopItems.length() > 0){
        QDrag* pDrag = new QDrag(this);
        pDrag->setMimeData(mimeData);
        pDrag->setPixmap(dragPixmap);
        QRect borderRect = getCheckedBorderRect();
        pDrag->setHotSpot(QPoint(-borderRect.x() + QCursor::pos().x(), -borderRect.y() + QCursor::pos().y() ));
        Qt::DropAction action = pDrag->exec(Qt::MoveAction | Qt::CopyAction, Qt::MoveAction);
        if (action == Qt::MoveAction){
            if (!m_isGridOn){
                foreach (DesktopItemPointer pItem, m_checkedDesktopItems) {
                    QPoint newPos = pItem->pos() + mapFromGlobal(QCursor::pos()) - m_pressedEventPos;
                    pItem->move(newPos);
                    m_desktopItemManager->getItems().removeOne(pItem);
                }
                m_desktopItemManager->getItems().append(m_checkedDesktopItems);
                foreach (DesktopItemPointer pItem, m_desktopItemManager->getItems()) {
                    pItem->raise();
                }
            }else{
                DesktopItemPointer pUnderMouseDesktopItem  = getTopDesktopItemByPos(mapFromGlobal(QCursor::pos())); //鼠标位置是否存在DesktopItem
                if (m_checkedDesktopItems.contains(pUnderMouseDesktopItem)){
                    pUnderMouseDesktopItem = DesktopItemPointer(); //置空鼠标当前位置的DesktopItem
                }

                if (!pUnderMouseDesktopItem.isNull()){ //鼠标当前位置的DesktopItem不为空

                }else{ //鼠标当前位置的DesktopItem为空


                    QList<DesktopItemPointer> outsideDesktopItems; //桌面范围外的desktopitems
                    QList<DesktopItemPointer> insideDesktopItems; // 桌面范围内的desktopitems
                    QList<DesktopItemPointer> occupiedDesktopItems; // 位置被占据了的desktopitems

                    foreach (DesktopItemPointer pItem, m_checkedDesktopItems) {//清空gridItem状态, 如果超出桌面范围, 设置相应gridItem无法容纳desktopitem
                        gridManager->getItemByPos(pItem->pos())->setDesktopItem(false);
                        QPoint newPos = pItem->pos() + mapFromGlobal(QCursor::pos()) - m_pressedEventPos;

                        QRect rect = pItem->geometry();
                        rect.moveTo(newPos);

                        bool isNewPosInGrid = gridManager->isRectInGrid(rect);
                        if (!isNewPosInGrid){
                            gridManager->getItemByPos(pItem->pos())->setDesktopItem(true);
                            outsideDesktopItems.append(pItem);
                        }else{
                            insideDesktopItems.append(pItem);
                        }
                    }

                    foreach (DesktopItemPointer pItem, insideDesktopItems) { //填充空位
                        QPoint newPos = pItem->geometry().center() + mapFromGlobal(QCursor::pos()) - m_pressedEventPos;
                        GridItemPointer newGridItem = gridManager->getBlankItemByPos(newPos);
                        if (!newGridItem.isNull()){
                            QPoint pos = newGridItem->getPos();
                            pItem->move(pos);
                            newGridItem->setDesktopItem(true);
                        }else{
                            occupiedDesktopItems.append(pItem);
                        }
                    }

                    foreach (DesktopItemPointer pItem, occupiedDesktopItems) { // 为位置被占据的desktopitem寻找可安放的girditem
                        QPoint newPos = pItem->pos() + mapFromGlobal(QCursor::pos()) - m_pressedEventPos;
                        GridItemPointer newGridItem = gridManager->getProperItemByPos(newPos);
                        if (!newGridItem.isNull()){
                            QPoint pos = newGridItem->getPos();
                            pItem->move(pos);
                            newGridItem->setDesktopItem(true);
                        }
                    }
                }
            }
            emit signalManager->desktopItemsSaved();
        }else{
            if (!m_dragLeave){
                if (m_ctrlPressed){
                    if (!m_TopDesktopItem->isChecked()){
                        checkRaiseItem(m_TopDesktopItem);
                    }
                }else{
                    if (!m_TopDesktopItem->isChecked()){
                        unCheckCheckedItems();
                        checkRaiseItem(m_TopDesktopItem);
                    }
                }
            }
        }
        pDrag->deleteLater();
    }
}


QPixmap DesktopFrame::getCheckedPixmap(){
    QFrame* F = new QFrame(this);
    F->setAttribute(Qt::WA_DeleteOnClose);
    F->setObjectName("DesktopFrame");
    F->setGeometry(qApp->desktop()->availableGeometry());
    foreach (DesktopItemPointer pItem, m_checkedDesktopItems) {
        DesktopItem* item = new DesktopItem(pItem->getDesktopIcon(),
                                       pItem->getDesktopName(), F);
        if (item->getTextEdit()->graphicsEffect())
            item->getTextEdit()->graphicsEffect()->setEnabled(false);
        bool flag = pItem->isShowSimpleMode();
        pItem->showSimpWrapName();
        item->resize(pItem->size());
        item->showSimpWrapName();
        if (flag){
            pItem->showSimpWrapName();
        }else{
            pItem->showFullWrapName();
        }
        item->move(mapToGlobal(pItem->pos()));
        item->setObjectName("DragChecked");
    }
    F->setStyleSheet(qApp->styleSheet());
    QRect viewRect(0, 0, qApp->desktop()->availableGeometry().width(), height());
    QPixmap ret = F->grab(getCheckedBorderRect());
    F->close();
    qDebug() << "getCheckedBorderRect" << getCheckedBorderRect();
    return ret;
}

QRect DesktopFrame::getCheckedBorderRect(){
    int minX = m_checkedDesktopItems.at(0)->pos().x();
    int maxX = m_checkedDesktopItems.at(0)->pos().x();
    int minY = m_checkedDesktopItems.at(0)->pos().y();
    int maxY = m_checkedDesktopItems.at(0)->pos().y();
    QList<int> xPoss;
    QList<int> yPoss;
    foreach (DesktopItemPointer pItem, m_checkedDesktopItems) {
        if (pItem->pos().x() < minX){
            minX = pItem->pos().x();
        }
        if (pItem->pos().x() > maxX){
            maxX = pItem->pos().x();
        }
        if (pItem->pos().y() < minY){
            minY = pItem->pos().y();
        }
        if (pItem->pos().y() > maxY){
            maxY = pItem->pos().y();
        }
    }
    int w = m_checkedDesktopItems.at(0)->width();
    int h = m_checkedDesktopItems.at(0)->height();
    return QRect(minX, minY, maxX - minX + w, maxY - minY + h);
}

void DesktopFrame::mouseReleaseEvent(QMouseEvent *event){
    clearFocus();
//    update();
    if (event->button() == Qt::LeftButton && !m_ctrlPressed){
        DesktopItemPointer pTopDesktopItem  = getTopDesktopItemByPos(m_pressedEventPos);
        if (m_desktopItemManager->isAppGroupBoxShowed()){
            emit signalManager->appGounpDetailClosed(event->pos());
            if (!pTopDesktopItem.isNull()){
                if (isAppGroup(pTopDesktopItem->getUrl())){
                    pTopDesktopItem->setChecked(true);
                }
            }
        }else{
            if (!pTopDesktopItem.isNull()){
                if (isAppGroup(pTopDesktopItem->getUrl())){
                    unCheckCheckedItems();
                    emit pTopDesktopItem->setHover(true);
                    emit signalManager->appGounpDetailShowed(pTopDesktopItem, event->pos());
                }
            }
        }
    }
    QFrame::mouseReleaseEvent(event);
}

void DesktopFrame::mouseMoveEvent(QMouseEvent *event){
    if (m_isDragStarted){
        emit signalManager->appGounpDetailClosed();
        startDrag();
    }else{
        int x = m_pressedEventPos.x();
        int y = m_pressedEventPos.y();
        int width = event->pos().x() - x;
        int height = event->pos().y() -y;
        m_selectRect = QRect(x, y , width, height);
        update();
        m_mouseMoveCheckTimer->start();
//        handleMouseMoveCheckItems();
    }
    QFrame::mouseMoveEvent(event);
}

void DesktopFrame::handleMouseMoveCheckItems(){
    checkDesktopItemsByRect(m_selectRect);
}

void DesktopFrame::mouseDoubleClickEvent(QMouseEvent *event){
    DesktopItemPointer pTopDesktopItem  = getTopDesktopItemByPos(event->pos());
    if (!pTopDesktopItem.isNull()){
        unCheckCheckedItems();
        checkRaiseItem(pTopDesktopItem);
        if (!isAppGroup(pTopDesktopItem->getUrl())){
            qDebug() << "mouseDoubleClickEvent" << pTopDesktopItem->getDesktopItemInfo().URI;
            emit signalManager->openFile(pTopDesktopItem->getDesktopItemInfo());
        }
    }
    QFrame::mouseDoubleClickEvent(event);
}

void DesktopFrame::paintEvent(QPaintEvent *event){
    Q_UNUSED(event)
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    if (m_isGridBackgoundOn){
        int rowCount = gridManager->getRowCount();
        int columnCount = gridManager->getColumnCount();
        foreach (GridListPointer gridlist, gridManager->getItems()) {
            foreach (GridItemPointer pGridItem, *gridlist) {
                int row = pGridItem->getRow();
                int column = pGridItem->getColumn();
                int _c = 255 * (row + column * rowCount) / (columnCount * rowCount);
                if (_c >= 255){
                    _c = 255;
                }
                QColor color(_c, _c, _c, 100);
                painter.fillRect(pGridItem->getRect(), color);
            }
        }
    }
    if (m_isSelectable){
        painter.setPen(QPen(QColor(255, 255, 255, 50), 2));
        painter.setBrush(QColor(0, 0 , 0, 63));
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.drawRect(m_selectRect);
    }
    QFrame::paintEvent(event);
}


DesktopFrame::~DesktopFrame()
{

}
