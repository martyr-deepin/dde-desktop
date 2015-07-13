#ifndef SIGNALMANAGER_H
#define SIGNALMANAGER_H

#include "desktopitem.h"
#include "gridmanager.h"
#include "dbusinterface/dbustype.h"
#include <QtCore>


class SignalManager: public QObject
{
    Q_OBJECT
public:
    static SignalManager* instance();

signals:
    void desktopItemsLoaded(EntryInfoObjList& entryInfoObjList);
    void desktopItemsSaved();

    void gridModeChanged(bool mode); /*true: grid on; false: grid off*/
    void gridOnResorted(); /*sort desktop item when grid on */

    void gridSizeTypeChanged(SizeType type);
    void sortedModeChanged(QDir::SortFlag flag);

    /*keyboard signal*/
    void keyUpPressed();
    void keyDownPressed();
    void keyLeftPressed();
    void keyRightPressed();

    void keyShiftLeftPressed();
    void keyShiftRightPressed();
    void keyShiftUpPressed();
    void keyShiftDownPressed();

    void keyCtrlAPressed();

    /*context menu signal*/
    void contextMenuShowed(DesktopItemPointer pItem, QPoint pos);
    void contextMenuShowed(const QList<DesktopItemPointer>& pCheckItems, const DesktopItemPointer& pItem, QPoint pos);

    void sendItemDesktopBack(QString itemId);

    /*update desktop icon*/
    void desktopItemsChanged(DesktopItemInfoMap& desktopnItems);

    /*monitor desktop folder*/
    void itemCreated(QString url);
    void itemDeleted(QString url);
    void itemMoved(QString oldUrl, QString newUrl);

private:
    SignalManager();
    ~SignalManager();
    static SignalManager* m_instance;
    Q_DISABLE_COPY(SignalManager)
};

#endif // SIGNALMANAGER_H
