#ifndef DESKTOPITEMMANAGER_H
#define DESKTOPITEMMANAGER_H

#include <QtCore>
#include "desktopitem.h"
#include "griditem.h"
#include "gridmanager.h"
#include "dbusinterface/dbustype.h"
#include "widgets/arrowrectangle.h"
#include "appgroupbox.h"

class DesktopItemManager : public QObject
{
    Q_OBJECT
public:
    DesktopItemManager(QObject* parent=0);
    ~DesktopItemManager();

    void loadDesktopItems();
    void initComputerItem();
    void initTrashItem();
    void initConnect();

    DesktopItemPointer getItemByPos(QPoint pos);
    DesktopItemPointer getShoudBeMovedItem();
    DesktopItemPointer createItem(const DesktopItemInfo& fileInfo);
    QList<DesktopItemPointer> getItems();

    QDir::SortFlag getSortFlag();
    QString getDesktopDisplayName(const DesktopItemInfo& desktopItemInfo);
    static QString decodeUrl(QString url);

signals:

public slots:
    void addItems(DesktopItemInfoMap& desktopInfoMap);
    void addItem(const DesktopItemInfo& fileInfo, int index);
    void addItem(const DesktopItemInfo& fileInfo);
    void updateAppGounpItem(QString group_url, DesktopItemInfoMap& appItems);
    void showAppGroupDetail(DesktopItemPointer& pItem, QPoint pos);
    void updateAppGroupDetail(DesktopItemPointer pItem);
    void closeAppGroupDetail(QPoint pos);
    void closeAppGroupDetail();
    void deleteItem(QString url);
    void saveItems();
    void changeSizeByGrid();
    void sortedByFlags(QDir::SortFlag flag);
    void sortedByKey(QString key);
    void resort();

    void setShoudByMovedItem(DesktopItemPointer pItem);
    void setShoudByMovedItemByUrl(QString url);
    void renameDesktopItem(DesktopItemInfo& desktopItemInfo);

private:
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
