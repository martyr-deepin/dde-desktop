#ifndef DESKTOPITEMMANAGER_H
#define DESKTOPITEMMANAGER_H

#include <QtCore>
#include "desktopitem.h"
#include "griditem.h"
#include "gridmanager.h"

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



signals:

public slots:
    void changeSizeByGrid();
    void sortedByFlags(QDir::SortFlag flag);
    void sortedByName();
    void sortedByTime();
    void sortedBySize();
    void sortedByType();
    void resort();

private:
    DesktopItemPointer m_pComputerItem;
    DesktopItemPointer m_pTrashItem;
    QWidget* m_parentWindow;
    QDir::SortFlag m_sortFlag;
    QList<DesktopItemPointer> m_list_pItems;
    QMap<QString, DesktopItemPointer> m_pItems;
};

#endif // DESKTOPITEMMANAGER_H
