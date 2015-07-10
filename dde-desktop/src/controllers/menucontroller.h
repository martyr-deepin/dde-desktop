#ifndef MENUCONTROLLER_H
#define MENUCONTROLLER_H


#include <QtCore>
#include "views/desktopitem.h"
#include "dbusinterface/menumanager_interface.h"
#include "dbusinterface/showmenu_interface.h"

#define MenuManager_service "com.deepin.menu"
#define MenuManager_path "/com/deepin/menu"

class MenuController : public QObject
{
    Q_OBJECT
public:
    QString MenuCreate_path = "";
    explicit MenuController(QObject *parent = 0);
    ~MenuController();

    void initConnect();

signals:

public slots:

    void showMenuByDesktopItem(DesktopItemPointer pItem, QPoint pos);
    void showMenuByDesktopItem(const QList<DesktopItemPointer>& pCheckItems, const DesktopItemPointer& pItem, QPoint pos);

    void createMenu(DesktopItemPointer pItem, QPoint pos);
    void destroyMenu(QString path);
    void menuItemInvoked(QString itemId);
    void handleSelectedItem(QString itemId);
private:
    MenumanagerInterface* m_menuManagerInterface;
    ShowmenuInterface* m_showmenuInterface;

    QString createMenuContent(QStringList createmenupath);
    QString registerMenu();
    QString JsonToQString(QPoint pos, QString menucontent);
    void showMenu(const QString path, QString content);
};

#endif // MENUCONTROLLER_H
