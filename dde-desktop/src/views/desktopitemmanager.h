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
    void initDesktopFolder();

    void initConnect();

    QList<DesktopItemPointer> getItems();

    QDir::SortFlag getSortFlag();


signals:

public slots:
    void addItems(EntryInfoObjList& entryInfoObjList);
    void addItem(EntryInfoObj fileInfo, int index);
    void saveItems();
    void changeSizeByGrid();
    void sortedByFlags(QDir::SortFlag flag);
    void resort();

private:
//    QFileSystemWatcher* m_fileSystemWatcher;
    QSettings m_settings;
    DesktopItemPointer m_pComputerItem;
    DesktopItemPointer m_pTrashItem;
    QWidget* m_parentWindow;
    QDir::SortFlag m_sortFlag;
    QList<DesktopItemPointer> m_list_pItems;
    QMap<QString, DesktopItemPointer> m_pItems;
};

#endif // DESKTOPITEMMANAGER_H
