#ifndef DESKTOPITEMMANAGER_H
#define DESKTOPITEMMANAGER_H

#include <QtCore>
#include "desktopfwd.h"
#include "desktopenums.h"
#include "dbusinterface/dbustype.h"

class AppGroupBox;
class QTimer;

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
    QList<DesktopItemPointer> getSortedItems();
    QList<DesktopItemPointer> getCheckedSortedItems();
    QList<DesktopItemPointer> getItemsByStartEnd(QPoint startPos, QPoint endPos);
    QDir::SortFlags getSortFlag();
    QString getDesktopDisplayName(DesktopItemInfo& desktopItemInfo);

    bool isAppGroupBoxShowed();

    void checkPageCount();

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
    void sortedByFlags(QDir::SortFlags flag);
    void sortedItems();
    void sortedByName();
    void sortedByKey(QString key);
    void resort();

    void setShoudBeMovedItem(DesktopItemPointer pItem);
    void setShoudBeMovedItemByUrl(QString url);
    void renameDesktopItem(DesktopItemInfo& desktopItemInfo);

    void handleDockModeChanged(int dockMode);

    void handleFileCreated(QString filename);

    void delayUpdateGridStatus();
    void updateGridStatus();
    void checkDesktopItemValid();

    void updateItems(QString url, const DesktopItemPointer& pItem);
    void handleItemsChanged();

    void handlePinyinChanged(const QList<DesktopItemInfo>& items);
private:
    QSettings m_settings;
    DesktopItemPointer m_pComputerItem;
    DesktopItemPointer m_pTrashItem;
    DesktopItemPointer m_shoudbeMovedItem;
    QWidget* m_parentWindow;
    QDir::SortFlags m_sortFlag;
    QList<DesktopItemPointer> m_list_pItems;
    QList<DesktopItemInfo> m_sortedPinyin_pItems;
    QMap<QString, DesktopItemPointer> m_pItems;
    AppGroupBox* m_appGroupBox=NULL;
    QTimer* m_gridUpdateTimer;
};

#endif // DESKTOPITEMMANAGER_H
