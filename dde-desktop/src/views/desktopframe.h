#ifndef DESKTOPFRAME__H
#define DESKTOPFRAME__H

#include "views/translucentframe.h"
#include "views/griditem.h"
#include "views/desktopitem.h"

#include <QtCore>
#include <QtGui>
#include <QtWidgets>

typedef QSharedPointer<GridItem> GridItemPointer;
typedef QList<GridItemPointer> GridList;
typedef QSharedPointer<GridList> GridListPointer;

typedef QSharedPointer<DesktopItem> DesktopItemPointer;

class DesktopFrame : public TranslucentFrame
{
    Q_OBJECT

public:
    DesktopFrame(QWidget *parent = 0);
    ~DesktopFrame();

    int gridItemWidth;
    int xGridSpacing;
    int yGridSpacing;
    int xGridStartPos;
    int yGridStartPos;
    int columnCount;
    int rowCount;

    void initGrid();
    void initDesktopItems();

    DesktopItemPointer getTopDesktopItemByPos(QPoint pos);

    void unCheckAllItems();
    void unCheckCheckedItems();
    void checkRaiseItem(DesktopItemPointer& item);
    QList<DesktopItemPointer> getCheckedDesktopItems();
    void checkDesktopItemsByRect(QRect rect);

    void startDrag();
    QPixmap getCheckedPixmap();

protected:
    void focusInEvent(QFocusEvent* event);
    void focusOutEvent(QFocusEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);

    void dragEnterEvent(QDragEnterEvent* event);
    void dragMoveEvent(QDragMoveEvent* event);
    void dropEvent(QDropEvent* event);

    void keyPressEvent(QKeyEvent* event);
    void paintEvent(QPaintEvent* event);

private:
    QPoint pressedEventPos;
    QRect selectRect;
    bool isSelectable;
    DesktopItemPointer m_TopDeskItem;
    QList<GridListPointer> m_gridItems;
    QList<DesktopItemPointer> m_desktopItems;
    QList<DesktopItemPointer> m_checkedDesktopItems;
};

#endif // DESKTOPFRAME__H
