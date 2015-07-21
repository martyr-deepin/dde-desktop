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
    void contextMenuShowed(QString url, QPoint pos);
    void contextMenuShowed(DesktopItemPointer pItem, QPoint pos);
    void contextMenuShowed(const QList<DesktopItemPointer>& pCheckItems, const DesktopItemPointer& pItem, QPoint pos);

    /*desktop menu*/
    void sortByKey(QString key);

    /*double open file*/
    void openFile(DesktopItemInfo info);

    /*trash files*/
    void trashEmptyChanged(bool flag);
    void trashingAboutToExcute(QStringList files); /*used for keyboard delete/ shift delete*/
    void trashingAboutToAborted();
    void trashingFileChanged(QString file);
    void deletingFileChanged(QString file);
    void processAmountChanged(qlonglong progress, ushort info);

    /*cut files*/
    void filesCuted(QStringList urls);
    void cancelFilesCuted(QStringList urls);

    /*paste files*/
    void moveFilesExcuted(QStringList files, QString destination);
    void moveJobAboutToAbort();
    void movingFileChaned(QString file);
    void movingProcessAmountChanged(qlonglong progress, ushort info);

    /*copy files*/
    void copyFilesExcuted(QStringList files, QString destination);
    void copyJobAboutToAbort();
    void copyingFileChaned(QString file);
    void copyingProcessAmountChanged(qlonglong progress, ushort info);

    /*update desktop icon like computer trash....*/
    void desktoItemIconUpdated(QString url, QString iconUl, uint size);

    /*update app group desktop icon*/
    void destinationAppGroupTryCreated(DesktopItemPointer& pItem);
    void requestCreatingAppGroup(QStringList urls);
    void requestMergeIntoAppGroup(QStringList urls, QString group_url);

    void desktopItemsChanged(DesktopItemInfoMap& desktopnItems);
    void appGounpItemsChanged(QString group_url, DesktopItemInfoMap& appItems);

    /*app group detail show*/
    void appGounpDetailShowed(DesktopItemPointer& pItem, QPoint pos);
    void appGounpDetailClosed(QPoint pos);
    void appGounpDetailClosed();

    /*monitor desktop folder*/
    void itemCreated(const DesktopItemInfo& fileInfo);
    void itemDeleted(QString url);

    void itemShoudBeMoved(QString url);
    void itemMoved(DesktopItemInfo& fileInfo);

private:
    SignalManager();
    ~SignalManager();
    static SignalManager* m_instance;
    Q_DISABLE_COPY(SignalManager)
};

#endif // SIGNALMANAGER_H
