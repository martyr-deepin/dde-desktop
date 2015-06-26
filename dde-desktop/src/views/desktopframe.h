#ifndef DESKTOPFRAME__H
#define DESKTOPFRAME__H

#include "views/translucentframe.h"
#include "views/griditem.h"
#include "views/desktopitem.h"
#include "gridmanager.h"

#include <QtCore>
#include <QtGui>
#include <QtWidgets>

class DesktopFrame : public TranslucentFrame
{
    Q_OBJECT

public:
    DesktopFrame(QWidget *parent = 0);
    ~DesktopFrame();

    void initGrid();
    void initDesktopItems();

    void setGridByType(SizeType type);

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
    QPoint m_pressedEventPos;
    QRect m_selectRect;
    bool m_isSelectable;
    bool m_isOrdered;

    GridManager* m_gridManager=NULL;
    DoubleGridItemPointerList m_gridItems;
    QMap<QString, GridItemPointer> m_mapItems;

    DesktopItemPointer m_TopDeskItem;
    QList<DesktopItemPointer> m_desktopItems;
    QList<DesktopItemPointer> m_checkedDesktopItems;

};

#endif // DESKTOPFRAME__H
