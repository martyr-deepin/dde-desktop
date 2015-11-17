#include "gridmanager.h"
#include "global.h"
#include "dbusinterface/displayinterface.h"

GridManager::GridManager(QObject *parent) : QObject(parent)
{
    initConnect();
}


void GridManager::initConnect(){

}

void GridManager::clearDeskopItemsStatus(){
    foreach (GridItemPointer pItem, m_map_items.values()) {
        pItem->setDesktopItem(false);
    }
}

int GridManager::getRowCount(){
    return m_rowCount;
}

int GridManager::getColumnCount(){
    return m_columnCount;
}

int GridManager::getDesktopColumnCount(int desktopWidth){
    return (desktopWidth - m_leftMargin - m_rightMargin) / (m_itemWidth + m_xMinimumSpacing);
}

int GridManager::getDesktopRowCount(int desktopHeight){
    return (desktopHeight - m_topMargin - m_bottomMargin) / (m_itemHeight + m_yMinimumSpacing);
}


int GridManager::getItemWidth(){
    return m_itemWidth;
}

int GridManager::getItemHeight(){
    return m_itemHeight;
}

int GridManager::getPageCount(){
    return m_pageCount;
}

void GridManager::setPageCount(int count){
    m_pageCount = count;
}

QMap<QString, GridItemPointer> GridManager::getMapItems(){
    return m_map_items;
}

DoubleGridItemPointerList GridManager::getItems(){
    return m_list_items;
}

DoubleGridItemPointerList GridManager::generateItems(const int width, const int height,
                                                 const int itemWidth, const int itemHeight,
                                                 const int xMinimumSpacing, const int yMinimumSpacing,
                                                 const int leftMargin, const int topMargin,
                                                 const int rightMargin, const int bottomMargin){
    const QRect primaryRect  =  QRect(dbusController->getDisplayInterface()->primaryRect());
    qDebug() << primaryRect;
    int desktopWidth = primaryRect.width();
    int desktopHeight = primaryRect.height() - m_dockHeight;

    m_list_items.clear();
    m_map_items.clear();
    m_width = width;
    m_height = height;
    m_itemWidth = itemWidth;
    m_itemHeight = itemHeight;
    m_xMinimumSpacing = xMinimumSpacing;
    m_yMinimumSpacing = yMinimumSpacing;
    m_leftMargin = leftMargin;
    m_topMargin = topMargin;
    m_rightMargin = rightMargin;
    m_bottomMargin = bottomMargin;

    int desktopColumn = getDesktopColumnCount(desktopWidth);
    int desktopRow = getDesktopRowCount(desktopHeight);

    qDebug() << "desktopColumn:" << desktopColumn;
    qDebug() << "desktopRow:" << desktopRow;

    m_columnCount = desktopColumn * width / desktopWidth;
    m_rowCount = desktopRow * height / desktopHeight;

    m_xSpacing = (desktopWidth - m_leftMargin - m_rightMargin - m_itemWidth * desktopColumn)/ (desktopColumn - 1);
    m_ySpacing = (desktopHeight - m_topMargin - m_bottomMargin - m_itemHeight * desktopRow) / (desktopRow - 1);
    int x = m_leftMargin;
    for (int i=0; i< m_columnCount; i++){
        if (i>0 && i % desktopColumn == 0){
            x = i / desktopColumn * desktopWidth + m_leftMargin;
        }
        GridListPointer _gridlist = GridListPointer::create();
        m_list_items.append(_gridlist);
        for (int j=0; j< m_rowCount; j++){
            int y = (m_itemHeight + m_ySpacing) * j + m_topMargin;
            QRect rect = QRect(x, y, m_itemWidth, m_itemHeight);
            GridItemPointer item = GridItemPointer::create(j, i, rect);
            QString key = QString("%1-%2").arg(QString::number(x), QString::number(y));
            m_map_items.insert(key, item);
            _gridlist->append(item);
        }
        x =  x + m_itemWidth  + m_xSpacing;

    }
    clearDeskopItemsStatus();
    getRightBottomItem()->setMultiDesktopItemsIn(true);
    return m_list_items;
}

GridItemPointer GridManager::getBlankItem(){
    for(int column = 0; column < m_columnCount; column++){
        for (int row = 0; row < m_rowCount; row++){
            GridItemPointer pItem = m_list_items.at(column)->at(row);
            if (!pItem->hasDesktopItem()){
                return pItem;
            }
        }
    }
    return GridItemPointer();
}

GridItemPointer GridManager::getRightBottomItem(){
    GridItemPointer pItem = m_list_items.at(m_columnCount - 1)->at(m_rowCount - 1);
    return pItem;
}

GridItemPointer GridManager::getItemByPos(QPoint pos){
    QString key = QString("%1-%2").arg(QString::number(pos.x()), QString::number(pos.y()));
    if (m_map_items.contains(key)){
        return m_map_items.value(key);
    }
    return GridItemPointer();
}

bool GridManager::isRectInGrid(QRect rect){
    QRect desktopGridRect = QRect(m_leftMargin, m_topMargin, m_width, m_height);
    bool flag = desktopGridRect.intersects(rect);
    return flag;
}

GridItemPointer GridManager::getBlankItemByPos(QPoint pos){
    QRect rect(pos, QSize(m_itemWidth, m_itemHeight));
    if (isRectInGrid(rect)){
        foreach (GridItemPointer pItem, m_map_items.values()) {
            QRect rect = pItem->getRect();
            if (pItem->getRow() < m_rowCount - 1 || pItem->getColumn() < m_columnCount - 1){
                rect.adjust(0, 0, m_xSpacing, m_ySpacing);
            }
            if (rect.contains(pos)){
                if (!pItem->hasDesktopItem()){
                    return pItem;
                }
            }
        }
    }
    return GridItemPointer();
}

GridItemPointer GridManager::getProperItemByPos(QPoint pos){
    int x = pos.x();
    int y = pos.y();
    int row = y / (m_itemHeight + m_ySpacing);
    int column = x / (m_itemWidth + m_xSpacing);

    QRect rect(pos, QSize(m_itemWidth, m_itemHeight));
    if (isRectInGrid(rect)){
        if (row <= m_rowCount && column <= m_columnCount){
            GridItemPointer neareastItem  = getNeareastItem(row, column, pos);
            if (!neareastItem.isNull()){
                if (!neareastItem->hasDesktopItem()){
                    return neareastItem;
                }
            }
        }
    }
    return GridItemPointer();
}

GridItemPointer GridManager::getNeareastItem(int row, int column, QPoint pos){
    QMap<QString, GridItemPointer> _pItems;
    int startRow = row;
    int startColumn = column;
    int count = 2;
    int _column = 0;
    int _row = 0;
    int loopCount = 2;
    while (true) {
        _pItems.clear();
        _column = startColumn;
        if (_column >= 0){
            for (int i=0; i < count; i++){
                int _row = startRow + i;
                if (_row < m_rowCount && _column < m_columnCount && _row >=0){
                    GridItemPointer pItem = m_list_items.at(_column)->at(_row);
                    if (!pItem->hasDesktopItem()){
                        _pItems.insert(pItem->key(), pItem);
                    }
                }
            }
        }

        _row = startRow;
        if (_row >= 0){
            for (int i=1; i < count - 1; i++){
                int _column = startColumn + i;
                if (_row < m_rowCount && _column < m_columnCount && _column >=0){
                    GridItemPointer pItem = m_list_items.at(_column)->at(_row);
                    if (!pItem->hasDesktopItem()){
                        _pItems.insert(pItem->key(), pItem);
                    }
                }
            }
        }

        _column = startColumn + count - 1;
        if (_column >= 0){
            for (int i=0; i < count; i++){
                int _row = startRow + i;
                if (_row < m_rowCount && _column < m_columnCount && _row >=0){
                    GridItemPointer pItem = m_list_items.at(_column)->at(_row);
                    if (!pItem->hasDesktopItem()){
                        _pItems.insert(pItem->key(), pItem);
                    }
                }
            }
        }

        _row = startRow + count - 1;
        if(_row >= 0){
            for (int i=1; i < count - 1; i++){
                int _column = startColumn + i;
                if (_row < m_rowCount && _column < m_columnCount && _column >=0){
                    GridItemPointer pItem = m_list_items.at(_column)->at(_row);
                    if (!pItem->hasDesktopItem()){
                        _pItems.insert(pItem->key(), pItem);
                    }
                }
            }
        }

        int itemsCount = _pItems.count();
        GridItemPointerList pItemsList = _pItems.values();
        if (itemsCount > 0){
            if (itemsCount == 1){
                return pItemsList.at(0);
            }else{
                GridItemPointer pNeareastItem;
                int minDistance = 0;
                for (int i=0; i < itemsCount - 1; i++) {
                    QPoint dPoint1 = pItemsList.at(i)->getPos() - pos;
                    int d1 = dPoint1.x() * dPoint1.x() + dPoint1.y() * dPoint1.y();
                    if (i == 0){
                        minDistance = d1;
                    }
                    if (d1 <= minDistance){
                        minDistance = d1;
                        pNeareastItem = pItemsList.at(i);
                    }
                }
                return pNeareastItem;
            }
        }
        startRow -= 1;
        startColumn -= 1;
        count = 2 * loopCount;
        loopCount += 1;
        if (count > qMax(m_rowCount, m_columnCount)){
            return GridItemPointer();
        }
    }
}



DoubleGridItemPointerList GridManager::getItemsByType(SizeType type){
    if (type == SizeType::Small){
        return getSmallItems();
    }else if (type == SizeType::Middle){
        return getMiddleItems();
    }else if (type == SizeType::Large){
        return getLargeItems();
    }else{
        return getMiddleItems();
    }
    m_sizeType = type;
}

DoubleGridItemPointerList GridManager::getSmallItems(){
    const QRect primaryRect = QRect(dbusController->getDisplayInterface()->primaryRect());
    int desktopWidth = primaryRect.width() * m_pageCount;
    int desktopHeight = primaryRect.height() - m_dockHeight;
    DoubleGridItemPointerList ret;
    ret = generateItems(desktopWidth, desktopHeight, 72, 72, 10, 10, 10, 10, 0, 10);
    return ret;
}

DoubleGridItemPointerList GridManager::getMiddleItems(){
    const QRect primaryRect = QRect(dbusController->getDisplayInterface()->primaryRect());
    int desktopWidth = primaryRect.width() * m_pageCount;
    int desktopHeight = primaryRect.height() - m_dockHeight;
    DoubleGridItemPointerList ret;
    ret = generateItems(desktopWidth, desktopHeight, 100, 100, 10, 10, 10, 10, 0, 10);
    return ret;
}

DoubleGridItemPointerList GridManager::getLargeItems(){
    const QRect primaryRect = QRect(dbusController->getDisplayInterface()->primaryRect());
    int desktopWidth = primaryRect.width() * m_pageCount;
    int desktopHeight = primaryRect.height() - m_dockHeight;
    DoubleGridItemPointerList ret;
    ret = generateItems(desktopWidth, desktopHeight, 140, 140, 10, 10, 10, 10, 0, 10);
    return ret;
}


GridManager::~GridManager()
{

}

