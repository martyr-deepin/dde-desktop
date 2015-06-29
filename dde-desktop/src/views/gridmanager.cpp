#include "gridmanager.h"

GridManager* GridManager::m_instance = NULL;

GridManager::GridManager(QObject *parent) : QObject(parent)
{

}

GridManager* GridManager::instance()
{
    static QMutex mutex;
    if (!m_instance) {
        QMutexLocker locker(&mutex);
        if (!m_instance)
            m_instance = new GridManager;
    }
    return m_instance;
}


int GridManager::getRowCount(){
    return m_rowCount;
}

int GridManager::getColumnCount(){
    return m_columnCount;
}

int GridManager::getItemWidth(){
    return m_itemWidth;
}

int GridManager::getItemHeight(){
    return m_itemHeight;
}

QMap<QString, GridItemPointer> GridManager::getMapItems(){
    return m_mapItems;
}

DoubleGridItemPointerList GridManager::generateItems(const int width, const int height,
                                                 const int itemWidth, const int itemHeight,
                                                 const int xMinimumSpacing, const int yMinimumSpacing,
                                                 const int leftMargin, const int topMargin,
                                                 const int rightMargin, const int bottomMargin){
    m_gridItems.clear();
    m_mapItems.clear();
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

    m_columnCount = (m_width - m_leftMargin - m_rightMargin) / (m_itemWidth + m_xMinimumSpacing);
    m_rowCount = (m_height - m_topMargin - m_bottomMargin) / (m_itemHeight + m_yMinimumSpacing);

    m_xSpacing = (m_width - m_leftMargin - m_rightMargin - m_itemWidth * m_columnCount)/ (m_columnCount - 1);
    m_ySpacing = (m_height - m_topMargin - m_bottomMargin - m_itemHeight * m_rowCount) / (m_rowCount - 1);

    int x=m_leftMargin;
    int y=m_topMargin;

    for (int i=0; i< m_columnCount; i++){
        x = m_itemWidth * i  + m_xSpacing * i + m_leftMargin;
        GridListPointer _gridlist = GridListPointer::create();
        m_gridItems.append(_gridlist);
        for (int j=0; j< m_rowCount; j++){
            y = m_itemHeight * j + m_ySpacing * j  + m_topMargin;
            QRect rect = QRect(x, y, m_itemWidth, m_itemHeight);
            GridItemPointer item = GridItemPointer::create(j, i, rect);
            QString key = QString("%1-%2").arg(QString::number(x), QString::number(y));
            m_mapItems.insert(key, item);
            _gridlist->append(item);
        }
    }
    return m_gridItems;
}

GridItemPointer GridManager::getItemByPos(QPoint pos){
    QString key = QString("%1-%2").arg(QString::number(pos.x()), QString::number(pos.y()));
    if (m_mapItems.contains(key)){
        return m_mapItems.value(key);
    }
    return GridItemPointer();
}

GridItemPointer GridManager::getProperItemByPos(QPoint pos){
    int x = pos.x();
    int y = pos.y();

    if (x > m_leftMargin && x < m_width - m_rightMargin && y > m_topMargin && y < m_height - m_bottomMargin){
        int row = y / (m_itemHeight + m_ySpacing);
        int column = x / (m_itemWidth + m_xSpacing);

        if (row <= m_rowCount && column <= m_columnCount){
            GridItemPointer item = m_gridItems.at(column)->at(row);
//            qDebug() << row << column << item->hasDesktopItem();
            if (!item->hasDesktopItem()){
                return item;
            }
        }
    }
    return GridItemPointer();
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
}


DoubleGridItemPointerList GridManager::getSmallItems(){
    const QRect availableGeometry = QApplication::desktop()->availableGeometry();
    int desktopWidth = availableGeometry.width();
    int desktopHeight = availableGeometry.height();
    DoubleGridItemPointerList ret;
    ret = generateItems(desktopWidth, desktopHeight, 72, 72, 10, 10, 10, 10, 10, 10);
    return ret;
}

DoubleGridItemPointerList GridManager::getMiddleItems(){
    const QRect availableGeometry = QApplication::desktop()->availableGeometry();
    int desktopWidth = availableGeometry.width();
    int desktopHeight = availableGeometry.height();
    DoubleGridItemPointerList ret;
    ret = generateItems(desktopWidth, desktopHeight, 100, 100, 10, 10, 10, 10, 10, 10);
    return ret;
}

DoubleGridItemPointerList GridManager::getLargeItems(){
    const QRect availableGeometry = QApplication::desktop()->availableGeometry();
    int desktopWidth = availableGeometry.width();
    int desktopHeight = availableGeometry.height();
    DoubleGridItemPointerList ret;
    ret = generateItems(desktopWidth, desktopHeight, 140, 140, 10, 10, 10, 10, 10, 10);
    return ret;
}


GridManager::~GridManager()
{

}

