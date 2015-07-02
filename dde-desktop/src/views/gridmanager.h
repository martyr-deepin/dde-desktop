#ifndef GRIDMANAGER_H
#define GRIDMANAGER_H

#include "griditem.h"
#include <QtCore>
#include <QtGui>
#include <QtWidgets>

enum SizeType {
    Small,
    Middle,
    Large
};

class GridManager : public QObject
{
    Q_OBJECT

public:
    explicit GridManager(QObject *parent = 0);
    ~GridManager();

    static GridManager* instance();

    int getColumnCount();
    int getRowCount();
    int getItemWidth();
    int getItemHeight();
    bool isRectInGrid(QRect rect);
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
    static GridManager* m_instance;
    DoubleGridItemPointerList m_list_items;
    QMap<QString, GridItemPointer> m_map_items;
};

#endif // GRIDMANAGER_H
