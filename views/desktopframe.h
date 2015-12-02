#ifndef DESKTOPFRAME__H
#define DESKTOPFRAME__H

#include "desktopfwd.h"
#include <QtCore>
#include <QtGui>
#include <QtWidgets>

class DesktopItemManager;
class KeyEventManager;
class DragDropEventManager;

class DesktopFrame : public QFrame
{
    Q_OBJECT

public:
    DesktopFrame(QWidget *parent = 0);
    ~DesktopFrame();

    static bool IsMenuShowed;
    friend class KeyEventManager;

    void initItems();
    void initConnect();

    QSharedPointer<DesktopItemManager> getDesktopItemManager();
    DesktopItemPointer getTopDesktopItemByPos(QPoint pos);

    void unCheckAllItems();
    void unCheckCheckedItems();
    void checkRaiseItem(DesktopItemPointer pItem);
    void unCheckItem(DesktopItemPointer pItem);
    void checkDesktopItemsByRect(QRect rect);
    DesktopItemPointer getLastPressedCheckedDesktopItem();
    DesktopItemPointer getLastCheckedDesktopItem();
    QList<DesktopItemPointer> getCheckedDesktopItems();


    void startDrag();
    QPixmap getCheckedPixmap();

    SizeType getSizeType();
    bool isGridOn();
    bool isCtrlPressed();
    bool isShiftPressed();

    bool isMultiCheckedByMouse();
    bool isAllAppCheckedItems();

    QStringList getCheckedFiles();
    QMap<QString, QString> getCheckedFileDislpyNames();

    QPoint getAppGroupDestinationPos();

    QRect getCheckedBorderRect();

signals:
    void lastCheckedDesktopItemChanged(DesktopItemPointer pItem);
    void checkedDesktopItemsAdded(DesktopItemPointer pItem);
    void checkedDesktopItemsRemoved(DesktopItemPointer pItem);
    void multiCheckedByMouseChanged(bool flag);
    void positionChanged(QPoint pos);

public slots:
    void setCtrlPressed(bool pressed);
    void setShiftPressed(bool pressed);
    void changeGridBySizeType(SizeType type);
    void changeGridMode(bool mode);
    void setLastCheckedDesktopItem(DesktopItemPointer pItem);
    void setLastPressedCheckedDesktopItem(DesktopItemPointer pItem);
    void addCheckedDesktopItem(DesktopItemPointer pItem);
    void removeCheckedDesktopItem(DesktopItemPointer pItem);
    void setMultiCheckedByMouse(bool flag);
    void checkAllDesktopItems();
    void setAppGroupDestinationPos(QPoint pos);

    void handleMouseMoveCheckItems();

    void handleKeyCtrlAPressed();
    void handleKeyCtrlCPressed();
    void handleKeyCtrlVPressed();
    void handleKeyCtrlXPressed();

    void move(const QPoint &pos);
    void move(int x, int y);

    void setMenuShowed(bool flag);

protected:
    void focusInEvent(QFocusEvent* event);
    void focusOutEvent(QFocusEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseDoubleClickEvent(QMouseEvent* event);

    void dragEnterEvent(QDragEnterEvent* event);
    void dragMoveEvent(QDragMoveEvent* event);
    void dragLeaveEvent(QDragLeaveEvent* event);
    void dropEvent(QDropEvent* event);

    void paintEvent(QPaintEvent* event);

private:
    QPoint m_pressedEventPos;
    QPoint m_dragMovePos;
    QRect m_selectRect;
    bool m_isSelectable = false;
    bool m_multiCheckedByMouse = false;
    bool m_ctrlPressed = false;
    bool m_shiftPressed = false;

    bool m_isGridOn = false;
    bool m_dragLeave = false;

    bool m_isDragStarted = false;
    bool m_isGridBackgoundOn = false;

    SizeType m_sizeType;

    QSharedPointer<DesktopItemManager> m_desktopItemManager;
    QSharedPointer<KeyEventManager> m_keyEventManager;
    QSharedPointer<DragDropEventManager> m_dragDropEventManager;

    DoubleGridItemPointerList m_gridItems;
    QMap<QString, GridItemPointer> m_mapItems;

    DesktopItemPointer m_TopDesktopItem; // z order top item
    DesktopItemPointer m_lastPressedCheckDesktopItem; // last press checked item
    DesktopItemPointer m_lastCheckedDesktopItem; // last checked item
    DesktopItemPointer m_shiftPressFirstCheckedDesktopItem; // first checked item when shift pressed

    DesktopItemPointer m_destinationDesktopItem = DesktopItemPointer(); /*desktop item which will change to be app group*/
    QPoint m_appGroupDestinationPos = QPoint(-1, -1);

    QList<DesktopItemPointer> m_checkedDesktopItems;

    QTimer* m_mouseMoveCheckTimer;

};

#endif // DESKTOPFRAME__H
