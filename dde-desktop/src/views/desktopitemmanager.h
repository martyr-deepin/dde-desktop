#ifndef DESKTOPITEMMANAGER_H
#define DESKTOPITEMMANAGER_H

#include <QtCore>
#include "desktopitem.h"
#include "griditem.h"
#include "gridmanager.h"
#include "dbusinterface/dbustype.h"

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

    QList<DesktopItemPointer> getItems();

    QDir::SortFlag getSortFlag();

    static QString decodeUrl(QString url);

signals:

public slots:
    void addItems(DesktopItemInfoMap& desktopInfoMap);
    void addItem(DesktopItemInfo fileInfo, int index);
    void deleteItem(QString url);
    void saveItems();
    void changeSizeByGrid();
    void sortedByFlags(QDir::SortFlag flag);
    void resort();

private:
    QSettings m_settings;
    DesktopItemPointer m_pComputerItem;
    DesktopItemPointer m_pTrashItem;
    QWidget* m_parentWindow;
    QDir::SortFlag m_sortFlag;
    QList<DesktopItemPointer> m_list_pItems;
    QMap<QString, DesktopItemPointer> m_pItems;
};

#endif // DESKTOPITEMMANAGER_H
