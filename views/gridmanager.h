#ifndef GRIDMANAGER_H
#define GRIDMANAGER_H

#include "desktopfwd.h"
#include "desktopenums.h"
#include <QtCore>

class GridManager : public QObject
{
    Q_OBJECT

public:
    explicit GridManager(QObject *parent = 0);
    ~GridManager();

    static GridManager* instance();
    void initConnect();

    int getColumnCount();
    int getRowCount();
    int getDesktopRowCount();
    int getDesktopColumnCount();
    int getItemWidth();
    int getItemHeight();
    int getPageCount();
    bool isRectInGrid(QRect rect);
    GridItemPointer getBlankItem();
    GridItemPointer getRightBottomItem();
    GridItemPointer getItemByPos(QPoint pos);
    GridItemPointer getBlankItemByPos(QPoint pos);
    GridItemPointer getProperItemByPos(QPoint pos);
    GridItemPointer getNeareastItem(int row, int column, QPoint pos);
    QMap<QString, GridItemPointer> getMapItems();
    DoubleGridItemPointerList getItemsByType(SizeType type);
    DoubleGridItemPointerList getSmallItems();
    DoubleGridItemPointerList getMiddleItems();
    DoubleGridItemPointerList getLargeItems();
    DoubleGridItemPointerList generateItems(const int width, const int height,
                                  const int itemWidth, const int itemHeight,
                                  const int xMinimumSpacing, const int yMinimumSpacing,
                                  const int leftMargin, const int topMargin,
                                  const int rightMargin, const int bottomMargin);


    DoubleGridItemPointerList getItems();
signals:

public slots:
    void clearDeskopItemsStatus();
    void setPageCount(int count);

private:
    int m_width = 0;
    int m_height = 0;
    int m_itemWidth = 100;
    int m_itemHeight = 100;
    int m_xMinimumSpacing = 10;
    int m_yMinimumSpacing = 10;
    int m_xSpacing = 0;
    int m_ySpacing = 0;
    int m_leftMargin = 10;
    int m_topMargin = 10;
    int m_rightMargin = 10;
    int m_bottomMargin = 10;
    int m_columnCount = 0;
    int m_rowCount = 0;
    int m_pageCount = 1;
    int m_dockHeight = 70;
    SizeType m_sizeType;
    DoubleGridItemPointerList m_list_items;
    QMap<QString, GridItemPointer> m_map_items;
    Q_DISABLE_COPY(GridManager)
};

#endif // GRIDMANAGER_H
