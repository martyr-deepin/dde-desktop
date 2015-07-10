#include "views/desktopframe.h"
#include "widgets/util.h"
#include "global.h"

DesktopFrame::DesktopFrame(QWidget *parent)
    : TranslucentFrame(parent)
{
    setAcceptDrops(true);


    QSettings settings;
    settings.beginGroup("Desktop");
    bool isGridOn = settings.value("isGridOn", true).toBool();
    int sizeType = settings.value("sizeType", SizeType::Middle).toInt();
    settings.endGroup();

    m_isGridOn = isGridOn;
    m_sizeType = static_cast<SizeType>(sizeType);
    m_desktopItemManager = QSharedPointer<DesktopItemManager>::create(this);
    m_keyEventManager = QSharedPointer<KeyEventManager>::create(this);

    m_TopDesktopItem = DesktopItemPointer();
    m_lastPressedCheckDesktopItem = DesktopItemPointer();
    m_lastCheckedDesktopItem = DesktopItemPointer();

    initItems();
    initConnect();
    qApp->setStyleSheet(getQssFromFile(":/skin/qss/DesktopItem.qss"));
}

void DesktopFrame::initItems(){

    m_gridItems = gridManager->getItemsByType(m_sizeType);
    m_mapItems = gridManager->getMapItems();
    m_desktopItemManager->loadDesktopItems();
}

void DesktopFrame::initConnect(){
    connect(signalManager, SIGNAL(gridSizeTypeChanged(SizeType)),
            this, SLOT(changeGridBySizeType(SizeType)));
    connect(signalManager, SIGNAL(gridModeChanged(bool)),
            this, SLOT(changeGridMode(bool)));
    connect(this, SIGNAL(lastCheckedDesktopItemChanged(DesktopItemPointer)),
            this, SLOT(setLastCheckedDesktopItem(DesktopItemPointer)));
    connect(this, SIGNAL(checkedDesktopItemsAdded(DesktopItemPointer)),
            this, SLOT(addCheckedDesktopItem(DesktopItemPointer)));
    connect(this, SIGNAL(checkedDesktopItemsRemoved(DesktopItemPointer)),
            this, SLOT(removeCheckedDesktopItem(DesktopItemPointer)));
    connect(this, SIGNAL(multiCheckedByMouseChanged(bool)), this, SLOT(setMultiCheckedByMouse(bool)));
}

QSharedPointer<DesktopItemManager> DesktopFrame::getTopDesktopItemManager(){
    return m_desktopItemManager;
}

SizeType DesktopFrame::getSizeType(){
    return m_sizeType;
}

bool DesktopFrame::isGridOn(){
    return m_isGridOn;
}

void DesktopFrame::changeGridBySizeType(SizeType type){
    m_gridItems = gridManager->getItemsByType(type);
    m_mapItems = gridManager->getMapItems();
    m_sizeType = type;
    m_desktopItemManager->changeSizeByGrid();
    update();
}

void DesktopFrame::changeGridMode(bool mode){
    m_isGridOn = mode;
    if(m_isGridOn){
        gridManager->clearDeskopItemsStatus();
        emit signalManager->gridOnResorted();
    }
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
                emit pItem->checkedChanged(true);
                emit lastCheckedDesktopItemChanged(pItem);
            }
        }else{
            if (pItem->isChecked()){
                emit pItem->checkedChanged(false);
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

void DesktopFrame::unCheckAllItems(){
    foreach (DesktopItemPointer pItem, m_desktopItemManager->getItems()) {
        emit pItem->checkedChanged(false);
    }
}

void DesktopFrame::unCheckCheckedItems(){
    foreach (DesktopItemPointer pItem, m_checkedDesktopItems) {
        emit pItem->checkedChanged(false);
    }
    m_checkedDesktopItems.clear();
}


void DesktopFrame::checkRaiseItem(DesktopItemPointer& pItem){
    if (!pItem.isNull()){
        emit pItem->checkedChanged(true);
        emit lastCheckedDesktopItemChanged(pItem);
        m_lastPressedCheckDesktopItem = pItem;
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

DesktopItemPointer DesktopFrame::getLastCheckedDesktopItem(){
    return m_lastCheckedDesktopItem;
}

void DesktopFrame::setLastCheckedDesktopItem(DesktopItemPointer pItem){
    m_lastCheckedDesktopItem = pItem;
    addCheckedDesktopItem(pItem);
}

void DesktopFrame::addCheckedDesktopItem(DesktopItemPointer pItem){
    if (!m_checkedDesktopItems.contains(pItem)){
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

void DesktopFrame::focusInEvent(QFocusEvent *event){
    m_isSelectable = true;
    m_selectRect = QRect(0, 0, 0, 0);
    TranslucentFrame::focusInEvent(event);
}


void DesktopFrame::focusOutEvent(QFocusEvent *event){
    m_isSelectable = false;
    m_selectRect = QRect(0, 0, 0, 0);
    TranslucentFrame::focusOutEvent(event);
}


void DesktopFrame::dragEnterEvent(QDragEnterEvent *event){
    qDebug() << "enter";
    m_dragLeave = false;
    if (event->mimeData()->hasFormat("application/x-dnditemdata")){
        if (event->source() == this){
            event->setDropAction(Qt::MoveAction);
            event->accept();
        }else{
            event->ignore();
        }
    }else{
        event->ignore();
    }
}


void DesktopFrame::dragMoveEvent(QDragMoveEvent *event){
    foreach(DesktopItemPointer pItem, m_desktopItemManager->getItems()){
        if (pItem->geometry().contains(event->pos())){
            pItem->hoverChanged(true);
        }else{
            pItem->hoverChanged(false);
        }
    }

    m_dragMovePos = event->pos();

    if (event->mimeData()->hasFormat("application/x-dnditemdata")){
        if (event->source() == this){
            event->setDropAction(Qt::MoveAction);
            event->accept();
        }else{
            event->ignore();
        }
    }else{
        event->ignore();
    }
}

void DesktopFrame::dragLeaveEvent(QDragLeaveEvent *event){
    m_dragLeave = true;
    event->accept();
}

void DesktopFrame::dropEvent(QDropEvent *event){
    if (event->mimeData()->hasFormat("application/x-dnditemdata")){
        if (event->source() == this){    
             event->setDropAction(Qt::MoveAction);
             event->accept();
        }else{
            event->accept();//eptProposedAction();
        }
    }else{
        event->ignore();
    }
}


void DesktopFrame::mousePressEvent(QMouseEvent *event){
    m_pressedEventPos = event->pos();
    DesktopItemPointer pTopDesktopItem  = getTopDesktopItemByPos(m_pressedEventPos);
    if (event->button() == Qt::LeftButton){

        if (pTopDesktopItem.isNull()){
            setFocus();
            unCheckCheckedItems();
        }else{
            if (!pTopDesktopItem->isChecked()){
                unCheckCheckedItems();
                checkRaiseItem(pTopDesktopItem);
            }
            startDrag();
        }

    }else if (event->button() == Qt::RightButton){
        if (pTopDesktopItem.isNull()){
            unCheckCheckedItems();
            emit signalManager->contextMenuShowed(pTopDesktopItem, m_pressedEventPos);
        }else{
            if (m_checkedDesktopItems.contains(pTopDesktopItem) && m_checkedDesktopItems.count() > 1){
                emit signalManager->contextMenuShowed(m_checkedDesktopItems, pTopDesktopItem, m_pressedEventPos);
            }else{
                unCheckCheckedItems();
                checkRaiseItem(pTopDesktopItem);
                emit signalManager->contextMenuShowed(pTopDesktopItem, m_pressedEventPos);
            }
        }
    }

    TranslucentFrame::mousePressEvent(event);
}

void DesktopFrame::startDrag(){
    QByteArray itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);
    QMimeData* mimeData = new QMimeData;
    mimeData->setData("application/x-dnditemdata", itemData);

    QPixmap dragPixmap = getCheckedPixmap();

    if (m_checkedDesktopItems.length() > 0){
        QDrag* drag = new QDrag(this);
        drag->setMimeData(mimeData);
        drag->setPixmap(dragPixmap);
        drag->setHotSpot(QCursor::pos());
        Qt::DropAction action = drag->exec(Qt::MoveAction | Qt::CopyAction, Qt::MoveAction);
        if (action == Qt::MoveAction){

            if (!m_isGridOn){
                foreach (DesktopItemPointer pItem, m_checkedDesktopItems) {
                    QPoint newPos = pItem->pos() + QCursor::pos() - m_pressedEventPos;
                    pItem->move(newPos);
                    m_desktopItemManager->getItems().removeOne(pItem);
                }
                m_desktopItemManager->getItems().append(m_checkedDesktopItems);
                foreach (DesktopItemPointer pItem, m_desktopItemManager->getItems()) {
                    pItem->raise();
                }
            }else{
                DesktopItemPointer pUnderMouseDesktopItem  = getTopDesktopItemByPos(QCursor::pos()); //鼠标位置是否存在DesktopItem
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
                        QPoint newPos = pItem->pos() + QCursor::pos() - m_pressedEventPos;

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
                        QPoint newPos = pItem->geometry().center() + QCursor::pos() - m_pressedEventPos;
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
                        QPoint newPos = pItem->pos() + QCursor::pos() - m_pressedEventPos;
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
                unCheckCheckedItems();
                checkRaiseItem(m_TopDesktopItem);
            }
        }
    }

}


QPixmap DesktopFrame::getCheckedPixmap(){
    TranslucentFrame* F = new TranslucentFrame(this);
    F->resize(size());
    foreach (DesktopItemPointer pItem, m_checkedDesktopItems) {
        DesktopItem* item = new DesktopItem(pItem->getDesktopIcon(),
                                       pItem->getDesktopName(), F);
        item->resize(pItem->size());
        item->move(pItem->pos());
        item->setObjectName("Checked");
    }
    F->setStyleSheet(qApp->styleSheet());
    QPixmap ret = F->grab();
    F->close();
    return ret;
}

void DesktopFrame::mouseReleaseEvent(QMouseEvent *event){
    clearFocus();
    update();
    TranslucentFrame::mouseReleaseEvent(event);
}

void DesktopFrame::mouseMoveEvent(QMouseEvent *event){

    int x = m_pressedEventPos.x();
    int y = m_pressedEventPos.y();
    int width = event->pos().x() - x;
    int height = event->pos().y() -y;
    m_selectRect = QRect(x, y , width, height);
    update();
    checkDesktopItemsByRect(m_selectRect);
    TranslucentFrame::mouseMoveEvent(event);
}

void DesktopFrame::paintEvent(QPaintEvent *event){
    Q_UNUSED(event)
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    int rowCount = gridManager->getRowCount();
    int columnCount = gridManager->getColumnCount();
    foreach (GridListPointer gridlist, m_gridItems) {
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
    if (m_isSelectable){
        QColor color(0, 0, 0, 90);
        painter.fillRect(m_selectRect, color);
    }
    TranslucentFrame::paintEvent(event);
}


void DesktopFrame::keyPressEvent(QKeyEvent *event){

    if (event->key() == Qt::Key_Escape){
        close();
    }else if (event->key() == Qt::Key_1){
        emit signalManager->gridSizeTypeChanged(SizeType::Small);
    }else if (event->key() == Qt::Key_2){
        emit signalManager->gridSizeTypeChanged(SizeType::Middle);
    }else if (event->key() == Qt::Key_3){
        emit signalManager->gridSizeTypeChanged(SizeType::Large);
    }else if (event->key() == Qt::Key_4){
        emit signalManager->sortedModeChanged(QDir::Name);
    }else if (event->key() == Qt::Key_5){
        emit signalManager->sortedModeChanged(QDir::Size);
    }else if (event->key() == Qt::Key_6){
        emit signalManager->sortedModeChanged(QDir::Type);
    }else if (event->key() == Qt::Key_7){
        emit signalManager->sortedModeChanged(QDir::Time);
    }else if (event->key() == Qt::Key_F1){
        emit signalManager->gridModeChanged(!m_isGridOn);
    }else if (event->modifiers() == Qt::NoModifier && event->key() == Qt::Key_Up){
        emit signalManager->keyUpPressed();
    }else if (event->modifiers() == Qt::NoModifier && event->key() == Qt::Key_Down){
        emit signalManager->keyDownPressed();
    }else if (event->modifiers() == Qt::NoModifier && event->key() == Qt::Key_Left){
        emit signalManager->keyLeftPressed();
    }else if (event->modifiers() == Qt::NoModifier && event->key() == Qt::Key_Right){
        emit signalManager->keyRightPressed();
    }else if (event->modifiers() == Qt::ShiftModifier && event->key() == Qt::Key_Left){
        emit signalManager->keyShiftLeftPressed();
    }else if (event->modifiers() == Qt::ShiftModifier && event->key() == Qt::Key_Right){
        emit signalManager->keyShiftRightPressed();
    }else if (event->modifiers() == Qt::ShiftModifier && event->key() == Qt::Key_Up){
        emit signalManager->keyShiftUpPressed();
    }else if (event->modifiers() == Qt::ShiftModifier && event->key() == Qt::Key_Down){
        emit signalManager->keyShiftDownPressed();
    }

    TranslucentFrame::keyPressEvent(event);
}

DesktopFrame::~DesktopFrame()
{

}
