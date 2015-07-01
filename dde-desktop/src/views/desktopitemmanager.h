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
    DesktopItemManager();
    ~DesktopItemManager();

    void loadDesktopItems();
    void changeSizeByGrid();

    QList<DesktopItemPointer> getItems();

signals:

public slots:

private:

    QList<DesktopItemPointer> m_desktopItems;
};

#endif // DESKTOPITEMMANAGER_H
