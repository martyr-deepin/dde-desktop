#ifndef GRIDMANAGER_H
#define GRIDMANAGER_H

#include <QtCore>
#include <QtGui>
#include <QtWidgets>
#include "griditem.h"

typedef QSharedPointer<GridItem> GridItemPointer;
typedef QList<GridItemPointer> GridItemPointerList;
typedef QSharedPointer<GridItemPointerList> GridListPointer;
typedef QList<GridListPointer> DoubleGridItemPointerList;

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
    GridItemPointer getItemByPos(QPoint pos);
    GridItemPointer getProperItemByPos(QPoint pos);
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

signals:

public slots:


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
    DoubleGridItemPointerList m_gridItems;
    QMap<QString, GridItemPointer> m_mapItems;
};

#endif // GRIDMANAGER_H
