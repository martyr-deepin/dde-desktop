#ifndef DESKTOPFRAME__H
#define DESKTOPFRAME__H

#include "views/translucentframe.h"
#include "views/griditem.h"
#include "views/desktopitem.h"
#include "gridmanager.h"
#include "desktopitemmanager.h"
#include "controllers/menucontroller.h"

#include <QtCore>
#include <QtGui>
#include <QtWidgets>

class DesktopFrame : public TranslucentFrame
{
    Q_OBJECT

public:
    DesktopFrame(QWidget *parent = 0);
    ~DesktopFrame();

    void initItems();
    void initConnect();

    QSharedPointer<DesktopItemManager> getTopDesktopItemManager();
    DesktopItemPointer getTopDesktopItemByPos(QPoint pos);

    void unCheckAllItems();
    void unCheckCheckedItems();
    void checkRaiseItem(DesktopItemPointer& item);
    QList<DesktopItemPointer> getCheckedDesktopItems();
    void checkDesktopItemsByRect(QRect rect);

    void startDrag();
    QPixmap getCheckedPixmap();

    SizeType getSizeType();
    bool isGridOn();

public slots:
    void changeGridBySizeType(SizeType type);
    void changeGridMode(bool mode);

protected:
    void focusInEvent(QFocusEvent* event);
    void focusOutEvent(QFocusEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);

    void dragEnterEvent(QDragEnterEvent* event);
    void dragMoveEvent(QDragMoveEvent* event);
    void dragLeaveEvent(QDragLeaveEvent* event);
    void dropEvent(QDropEvent* event);

    void keyPressEvent(QKeyEvent* event);
    void paintEvent(QPaintEvent* event);

private:
    QPoint m_pressedEventPos;
    QPoint m_dragMovePos;
    QRect m_selectRect;
    bool m_isSelectable;
    bool m_isGridOn;
    bool m_dragLeave;
    SizeType m_sizeType;

    QSharedPointer<DesktopItemManager> m_desktopItemManager;

    DoubleGridItemPointerList m_gridItems;
    QMap<QString, GridItemPointer> m_mapItems;

    DesktopItemPointer m_TopDeskItem;
    QList<DesktopItemPointer> m_checkedDesktopItems;

};

#endif // DESKTOPFRAME__H
