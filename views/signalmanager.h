#ifndef SIGNALMANAGER_H
#define SIGNALMANAGER_H

#include "desktopfwd.h"
#include "desktopenums.h"
#include "dbusinterface/dbustype.h"
#include <QtCore>


class SignalManager: public QObject
{
    Q_OBJECT
public:
    explicit SignalManager(){}
    ~SignalManager(){}

signals:

    void desktopItemsSaved();

    void gridModeChanged(bool mode); /*true: grid on; false: grid off*/
    void gridOnResorted(); /*sort desktop item when grid on */

    void gridSizeTypeChanged(SizeType type);
    void sortedModeChanged(QDir::SortFlags flag);

    /*uncheck dekstop item by uyl*/
    void unCheckedItem(QString url);

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
    void keyCtrlCPressed();
    void keyCtrlVPressed();

    /*context menu signal*/
    void contextMenuShowed(QString url, QPoint pos);
    void contextMenuShowed(DesktopItemPointer pItem, QPoint pos);
    void contextMenuShowed(const QList<DesktopItemPointer>& pCheckItems, const DesktopItemPointer& pItem, QPoint pos);

    /*desktop menu*/
    void sortByKey(QString key);

    /*double open file*/
    void openFile(DesktopItemInfo info);
    /*drop open files*/
    void openFiles(DesktopItemInfo destinationDesktopItemInfo, QList<DesktopItemInfo> desktopItemInfos);
    void openFiles(DesktopItemInfo destinationDesktopItemInfo, QStringList urls);

    /*trash files*/
    void trashEmptyChanged(bool flag);
    void trashingAboutToExcute(QStringList files); /*used for keyboard delete/ shift delete*/
    void trashingAboutToAborted();
    void trashingFileChanged(QString file);
    void deletingFileChanged(QString file);
    void processAmountChanged(qlonglong progress, ushort info);

    void requestEmptyTrash();

    /*copy files*/
    void filesCopyed(QStringList urls);

    /*cut files*/
    void filesCuted(QStringList urls);
    void cancelFilesCuted(QStringList urls);

    /*paste files*/
    void pasteFilesToDesktop();
    void moveFilesExcuted(const QStringList& files, QString destination);
    void moveJobAboutToAbort();
    void movingFileChaned(QString file);
    void movingProcessAmountChanged(qlonglong progress, ushort info);

    /*dbus delete files*/
    void deleteFilesExcuted(const QStringList& files);
    void deleteJobAboutToAbort();
    void deletingFileChaned(QString file);
    void deletingProcessAmountChanged(qlonglong progress, ushort info);

    /*dbus copy files*/
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

    void desktopItemsChanged(DesktopItemInfoMap desktopnItems);
    void appGounpItemsChanged(QString group_url, DesktopItemInfoMap appItems);

    /*app group detail show*/
    void appGounpDetailShowed(DesktopItemPointer& pItem, QPoint pos);
    void appGounpDetailClosed(QPoint pos);
    void appGounpDetailClosed();

    /*monitor desktop folder*/
    void itemCreated(DesktopItemInfo fileInfo);
    void itemDeleted(QString url);

    void itemShoudBeMoved(QString url);
    void itemMoved(DesktopItemInfo& fileInfo);

    /*F2 renamed*/
    void renameStarted();
    void renameFinished();
    void renameJobCreated(QString url, QString newName);

    /*context menu renamed*/
    void requestRenamed(QString url);

    /*dock mode change*/
    void dockModeChanged(int dockMode);

    /*create folder/file/template finised*/
    void fileCreated(QString filename);

    /*request desktop items from dbus*/
    void startRequest();
    void stopRequest();

    /*update gridmanager status when move desktopitem*/
    void gridStatusUpdated();

    /*screen geometry changed*/
    void screenGeometryChanged();

    /*gtk wather monitor icon theme changed signal*/
    void gtkIconThemeChanged();

    /*dbus to copy job dialog */
    void copyJobAdded(const QMap<QString, QString>& jobDetail);
    void copyJobRemoved(const QMap<QString, QString>& jobDetail);

    void copyJobDataUpdated(const QMap<QString, QString>& jobDetail,
                           const QMap<QString, QString>& data);

    /*copy job dialog to dbus*/
    void abortCopyTask(const QMap<QString, QString>& jobDetail);


    /*dbus to move job dialog */
    void moveJobAdded(const QMap<QString, QString>& jobDetail);
    void moveJobRemoved(const QMap<QString, QString>& jobDetail);

    void moveJobDataUpdated(const QMap<QString, QString>& jobDetail,
                           const QMap<QString, QString>& data);

    /*move job dialog to dbus*/
    void abortMoveTask(const QMap<QString, QString>& jobDetail);

    /*delete confirm dialog to delete woker*/
    void filesDeleted(const QStringList& files);

    /*dbus to delete job dialog */
    void deleteJobAdded(const QMap<QString, QString>& jobDetail);
    void deleteJobRemoved(const QMap<QString, QString>& jobDetail);

    void deleteJobDataUpdated(const QMap<QString, QString>& jobDetail,
                           const QMap<QString, QString>& data);

    /*move job dialog to dbus*/
    void abortDeleteTask(const QMap<QString, QString>& jobDetail);


private:
    Q_DISABLE_COPY(SignalManager)
};

#endif // SIGNALMANAGER_H
