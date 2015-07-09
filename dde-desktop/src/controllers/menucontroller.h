#ifndef MENUCONTROLLER_H
#define MENUCONTROLLER_H


#include <QtCore>
#include "views/desktopitem.h"

class MenuController : public QObject
{
    Q_OBJECT
public:
    explicit MenuController(QObject *parent = 0);
    ~MenuController();

    void initConnect();

signals:

public slots:
    void showMenuByDesktopItem(DesktopItemPointer pItem, QPoint pos);
    void showMenuByDesktopItem(const QList<DesktopItemPointer>& pCheckItems, const DesktopItemPointer& pItem, QPoint pos);
};

#endif // MENUCONTROLLER_H
