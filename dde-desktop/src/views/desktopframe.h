#ifndef DESKTOPFRAME__H
#define DESKTOPFRAME__H

#include "views/translucentframe.h"
#include "views/griditem.h"
#include "views/desktopitem.h"
#include "gridmanager.h"
#include "desktopitemmanager.h"
#include "keyeventmanager.h"
#include "controllers/menucontroller.h"

#include <QtCore>
#include <QtGui>
#include <QtWidgets>

class DesktopFrame : public QFrame
{
    Q_OBJECT

public:
    DesktopFrame(QWidget *parent = 0);
    ~DesktopFrame();

    friend class KeyEventManager;

    void initItems();
    void initConnect();

    QSharedPointer<DesktopItemManager> getTopDesktopItemManager();
    DesktopItemPointer getTopDesktopItemByPos(QPoint pos);

    void unCheckAllItems();
    void unCheckCheckedItems();
    void checkRaiseItem(DesktopItemPointer& pItem);
    void unCheckItem(DesktopItemPointer& pItem);
    void checkDesktopItemsByRect(QRect rect);
    DesktopItemPointer getLastPressedCheckedDesktopItem();
    DesktopItemPointer getLastCheckedDesktopItem();
    QList<DesktopItemPointer> getCheckedDesktopItems();


    void startDrag();
    QPixmap getCheckedPixmap();

    SizeType getSizeType();
    bool isGridOn();
    bool isCtrlPressed();

    bool isMultiCheckedByMouse();
    bool isAllAppCheckedItems();

    QStringList getCheckedFiles();

    QPoint getAppGroupDestinationPos();

signals:
    void lastCheckedDesktopItemChanged(DesktopItemPointer pItem);
    void checkedDesktopItemsAdded(DesktopItemPointer pItem);
    void checkedDesktopItemsRemoved(DesktopItemPointer pItem);
    void multiCheckedByMouseChanged(bool flag);

public slots:
    void setCtrlPressed(bool pressed);
    void resizeByPageCount(int pageCount);
    void changeGridBySizeType(SizeType type);
    void changeGridMode(bool mode);
    void setLastCheckedDesktopItem(DesktopItemPointer pItem);
    void setLastPressedCheckedDesktopItem(DesktopItemPointer pItem);
    void addCheckedDesktopItem(DesktopItemPointer pItem);
    void removeCheckedDesktopItem(DesktopItemPointer pItem);
    void setMultiCheckedByMouse(bool flag);
    void checkAllDesktopItems();
    void setAppGroupDestinationPos(QPoint pos);

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
    bool m_isSelectable;
    bool m_multiCheckedByMouse;
    bool m_ctrlPressed;

    bool m_isGridOn;
    bool m_dragLeave;

    bool m_isDragStarted;
    bool m_isGridBackgoundOn = true;

    SizeType m_sizeType;

    QSharedPointer<DesktopItemManager> m_desktopItemManager;
    QSharedPointer<KeyEventManager> m_keyEventManager;

    DoubleGridItemPointerList m_gridItems;
    QMap<QString, GridItemPointer> m_mapItems;

    DesktopItemPointer m_TopDesktopItem; // z order top item
    DesktopItemPointer m_lastPressedCheckDesktopItem; // last press checked item
    DesktopItemPointer m_lastCheckedDesktopItem; // last checked item

    DesktopItemPointer m_destinationDesktopItem = DesktopItemPointer(); /*desktop item which will change to be app group*/
    QPoint m_appGroupDestinationPos = QPoint(-1, -1);

    QList<DesktopItemPointer> m_checkedDesktopItems;

};

#endif // DESKTOPFRAME__H
