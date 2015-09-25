#ifndef DESKTOPITEMMANAGER_H
#define DESKTOPITEMMANAGER_H

#include <QtCore>
#include "desktopfwd.h"
#include "desktopenums.h"
#include "dbusinterface/dbustype.h"

class AppGroupBox;

class DesktopItemManager : public QObject
{
    Q_OBJECT
public:
    DesktopItemManager(QObject* parent=0);
    ~DesktopItemManager();
    void initComputerItem();
    void initTrashItem();
    void initConnect();

    DesktopItemPointer getItemByPos(QPoint pos);
    DesktopItemPointer getItemByUrl(QString url);
    DesktopItemPointer getShoudBeMovedItem();
    DesktopItemPointer createItem(DesktopItemInfo& fileInfo);
    QList<DesktopItemPointer> getItems();

    QDir::SortFlag getSortFlag();
    QString getDesktopDisplayName(DesktopItemInfo& desktopItemInfo);
    static QString decodeUrl(QString url);

    bool isAppGroupBoxShowed();

    void checkPageCount();
    int getPageCount();

signals:

public slots:
    void loadComputerTrashItems();
    void clearComputerTrashItems();
    void unCheckedItem(QString url);
    void addItems(DesktopItemInfoMap desktopInfoMap);
    void addItem(DesktopItemInfo fileInfo, int index);
    void addItem(DesktopItemInfo fileInfo);
    void updateDesktopItemIcon(QString url, QString iconUl, uint size);
    void updateAppGounpItem(QString group_url, DesktopItemInfoMap appItems);
    void showAppGroupDetail(DesktopItemPointer& pItem, QPoint pos);
    void updateAppGroupDetail(DesktopItemPointer pItem);
    void closeAppGroupDetail(QPoint pos);
    void closeAppGroupDetail();
    void deleteItem(QString url);
    void cutItems(QStringList urls);
    void cancelCutedItems(QStringList urls);
    void saveItems();
    void changeSizeByGrid(SizeType type);
    void sortedByFlags(QDir::SortFlag flag);
    void sortedByKey(QString key);
    void resort();

    void setShoudBeMovedItem(DesktopItemPointer pItem);
    void setShoudBeMovedItemByUrl(QString url);
    void renameDesktopItem(DesktopItemInfo& desktopItemInfo);

    void handleDockModeChanged(int dockMode);

    void handleFileCreated(QString filename);

private:
    int m_pagecount = 0;
    QSettings m_settings;
    DesktopItemPointer m_pComputerItem;
    DesktopItemPointer m_pTrashItem;
    DesktopItemPointer m_shoudbeMovedItem;
    QWidget* m_parentWindow;
    QDir::SortFlag m_sortFlag;
    QList<DesktopItemPointer> m_list_pItems;
    QMap<QString, DesktopItemPointer> m_pItems;
    AppGroupBox* m_appGroupBox=NULL;
};

#endif // DESKTOPITEMMANAGER_H
