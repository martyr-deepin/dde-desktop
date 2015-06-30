#include "views/desktopframe.h"
#include "widgets/util.h"
#include "global.h"

DesktopFrame::DesktopFrame(QWidget *parent)
    : TranslucentFrame(parent)
{

    setAcceptDrops(true);
    m_isOrdered = true;
    initGrid();
    initDesktopItems();

    setGeometry(qApp->desktop()->availableGeometry());
    qApp->setStyleSheet(getQssFromFile(":/skin/qss/DesktopItem.qss"));
}

void DesktopFrame::initGrid(){
    setGridByType(SizeType::Middle);

}

void DesktopFrame::initDesktopItems(){
    int width = gridManager->getItemWidth();
    int height = gridManager->getItemHeight();
    int column = gridManager->getColumnCount() / 2;
    int row = gridManager->getRowCount() / 2;

    for(int i=0; i<column; i++){
        for (int j=0; j< row; j++){
            DesktopItemPointer  desktopItem = DesktopItemPointer::create(":/skin/images/QFramer.png", QString::number(i * row + j), this);
            desktopItem->resize(width, height);
            m_desktopItems.append(desktopItem);

            GridItemPointer gridItem = m_gridItems.at(i)->at(j);
            if (!gridItem.isNull()){
                QRect rect = gridItem->getRect();
                desktopItem->move(rect.topLeft());
                gridItem->setDesktopItem(true);
            }
        }
    }
}

void DesktopFrame::setGridByType(SizeType type){
    m_gridItems = gridManager->getItemsByType(type);
    m_mapItems = gridManager->getMapItems();
    update();

    int itemWidth = gridManager->getItemWidth();
    int itemHeight = gridManager->getItemHeight();
    foreach (DesktopItemPointer pItem, m_desktopItems) {
            pItem->resize(itemWidth, itemHeight);
//            pItem->move(newGridItem->getPos());
//        }
    }
}

DesktopItemPointer DesktopFrame::getTopDesktopItemByPos(QPoint pos){
    for (int i=m_desktopItems.count() - 1; i >= 0; i--){
        if (m_desktopItems.at(i)->geometry().contains(pos)){
            m_TopDeskItem = m_desktopItems.at(i);
            return m_TopDeskItem;
        }
    }
    return DesktopItemPointer();
}

void DesktopFrame::checkDesktopItemsByRect(QRect rect){
    foreach (DesktopItemPointer pItem, m_desktopItems) {
        if (rect.intersects(pItem->geometry())){
            emit pItem->checkedChanged(true);
        }else{
            emit pItem->checkedChanged(false);
        }
    }
}

void DesktopFrame::unCheckAllItems(){
    foreach (DesktopItemPointer pItem, m_desktopItems) {
        emit pItem->checkedChanged(false);
    }
}

void DesktopFrame::unCheckCheckedItems(){
    foreach (DesktopItemPointer pItem, m_checkedDesktopItems) {
        emit pItem->checkedChanged(false);
    }
    m_checkedDesktopItems.clear();
}


void DesktopFrame::checkRaiseItem(DesktopItemPointer& item){
    if (!item.isNull()){
        emit item->checkedChanged(true);
        item->raise();

        bool flag = m_desktopItems.removeOne(item);
        if (flag){
            m_desktopItems.append(item);
        }
    }
}

QList<DesktopItemPointer> DesktopFrame::getCheckedDesktopItems(){
    m_checkedDesktopItems.clear();
    foreach (DesktopItemPointer pItem, m_desktopItems) {
        if (pItem->isChecked()){
            m_checkedDesktopItems.append(pItem);
        }
    }
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
    foreach(DesktopItemPointer pItem, m_desktopItems){
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
    qDebug() << event << "111";
}

void DesktopFrame::dropEvent(QDropEvent *event){
    if (event->mimeData()->hasFormat("application/x-dnditemdata")){
        if (event->source() == this){    
             event->setDropAction(Qt::MoveAction);
             event->accept();
        }else{
            event->acceptProposedAction();
        }
    }else{
        event->ignore();
    }
}


void DesktopFrame::mousePressEvent(QMouseEvent *event){
    m_pressedEventPos = event->pos();
    DesktopItemPointer pTopDesktopItem  = getTopDesktopItemByPos(m_pressedEventPos);
    m_checkedDesktopItems = getCheckedDesktopItems();
    if (event->button() == Qt::LeftButton){

        if (pTopDesktopItem.isNull()){
            setFocus();
            unCheckCheckedItems();
        }else{
            if (!pTopDesktopItem->isChecked()){
                checkRaiseItem(pTopDesktopItem);
            }

            if (m_checkedDesktopItems.indexOf(pTopDesktopItem) == -1){
                unCheckCheckedItems();
                m_checkedDesktopItems.append(pTopDesktopItem);
            }

            startDrag();
        }

    }else if (event->button() == Qt::RightButton){
        if (pTopDesktopItem.isNull()){
            unCheckCheckedItems();
            signalManager->contextMenuShowed(pTopDesktopItem, m_pressedEventPos);
        }else{
            checkRaiseItem(pTopDesktopItem);
            signalManager->contextMenuShowed(pTopDesktopItem, m_pressedEventPos);
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

            if (!m_isOrdered){
                foreach (DesktopItemPointer pItem, m_checkedDesktopItems) {
                    QPoint newPos = pItem->pos() + QCursor::pos() - m_pressedEventPos;
                    pItem->move(newPos);
                    m_desktopItems.removeOne(pItem);
                }
                m_desktopItems.append(m_checkedDesktopItems);
                foreach (DesktopItemPointer pItem, m_desktopItems) {
                    pItem->raise();
                }
            }else{
                DesktopItemPointer pUnderMouseDesktopItem  = getTopDesktopItemByPos(QCursor::pos()); //鼠标位置是否存在DesktopItem
                if (m_checkedDesktopItems.contains(pUnderMouseDesktopItem)){
                    pUnderMouseDesktopItem = DesktopItemPointer(); //置空鼠标当前位置的DesktopItem
                }

                if (!pUnderMouseDesktopItem.isNull()){ //鼠标当前位置的DesktopItem不为空

                }else{ //鼠标当前位置的DesktopItem为空


                    QList<DesktopItemPointer> outsideDesktopItems; //桌面范围内的desktopitems
                    QList<DesktopItemPointer> insideDesktopItems; // 桌面范围外的desktopitems
                    QList<DesktopItemPointer> occupiedDesktopItems; // 位置被占据了的desktopitems

                    foreach (DesktopItemPointer pItem, m_checkedDesktopItems) {//清空gridItem状态, 如果超出桌面范围, 设置相应gridItem无法容纳desktopitem
                        gridManager->getItemByPos(pItem->pos())->setDesktopItem(false);
                        QPoint newPos = pItem->geometry().center() + QCursor::pos() - m_pressedEventPos;
                        bool isnewPosInGrid = gridManager->isPosInGrid(newPos);
                        if (!isnewPosInGrid){
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
                        qDebug() << pItem->getDesktopName() << "位置被占据" << gridManager->getItemByPos(pItem->pos())->hasDesktopItem();
                        if (!newGridItem.isNull()){
                            QPoint pos = newGridItem->getPos();
                            pItem->move(pos);
                            newGridItem->setDesktopItem(true);
                        }
                    }
                }
            }
        }else{
            qDebug() << "==========";
//            unCheckCheckedItems();
//            checkRaiseItem(m_TopDeskItem);
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
        setGridByType(SizeType::Small);
    }else if (event->key() == Qt::Key_2){
        setGridByType(SizeType::Middle);
    }else if (event->key() == Qt::Key_3){
        setGridByType(SizeType::Large);
    }else if (event->key() == Qt::Key_F1){
        m_isOrdered = !m_isOrdered;
        emit signalManager->girdModeChanged(!m_isOrdered);
    }

    TranslucentFrame::keyPressEvent(event);
}

DesktopFrame::~DesktopFrame()
{

}
