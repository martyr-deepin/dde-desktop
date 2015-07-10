#include "menucontroller.h"
#include "views/global.h"

MenuController::MenuController(QObject *parent) : QObject(parent)
{
    QDBusConnection menu_dbus = QDBusConnection::sessionBus();

    m_menuManagerInterface = new MenumanagerInterface(MenuManager_service, MenuManager_path, menu_dbus);
    m_desktopBackInterface = new DesktopbackInterface(DesktopBack_service, DesktopBack_path, menu_dbus);
    MenuCreate_path = "";
    initConnect();
}
MenuController::~MenuController()
{
}
void MenuController::initConnect() {
    connect(signalManager, SIGNAL(contextMenuShowed(DesktopItemPointer,QPoint)), this, SLOT(createMenu(DesktopItemPointer,QPoint)));
    connect(signalManager, SIGNAL(sendItemDesktopBack(QString)), this, SLOT(handleSelectedItem(QString)));
}

void MenuController::createMenu(DesktopItemPointer pItem, QPoint pos){
    QStringList iconurl;
    if (!pItem.isNull()) {
        iconurl = QStringList(pItem->getUrl());
    }
    QString menucontent = createMenuContent(iconurl);
    QString menucontentfinal = JsonToQString(pos, menucontent);
    QString menucreatepath = registerMenu();
    MenuCreate_path = menucreatepath;
    showMenu(menucreatepath, menucontentfinal);
}
void MenuController::destroyMenu(QString menupath) {
    m_menuManagerInterface->UnregisterMenu(menupath);
}

QString MenuController::createMenuContent(QStringList createmenupath) {
    QDBusPendingReply<QString> menu_content_reply = m_desktopBackInterface->GenMenuContent(createmenupath);
    menu_content_reply.waitForFinished();
    if (!menu_content_reply.isError()){
        QString menu_content = menu_content_reply.argumentAt(0).toString();
        return menu_content;
    } else {
        return "";
    }
}
QString MenuController::JsonToQString(QPoint pos, QString menucontent) {
    QJsonObject menuObj
    {
        {"x", pos.x()},
        {"y", pos.y()},
        {"isDockMenu", false},
        {"menuJsonContent", menucontent}
    };
    return QString(QJsonDocument(menuObj).toJson());
}
QString MenuController::registerMenu() {
    QDBusPendingReply<QDBusObjectPath> menu_register_reply = m_menuManagerInterface->RegisterMenu();
    menu_register_reply.waitForFinished();
    if (!menu_register_reply.isError()) {
        return menu_register_reply.value().path();
    } else {
        return "";
    }
}
void MenuController::showMenu(const QString showmenu_path, QString menucontent) {

    QDBusConnection menu_dbus = QDBusConnection::sessionBus();
    m_showmenuInterface = new ShowmenuInterface(MenuManager_service, showmenu_path, menu_dbus);
    m_showmenuInterface->ShowMenu(menucontent);
    connect(m_showmenuInterface, SIGNAL(ItemInvoked(QString, bool)), this, SLOT(menuItemInvoked(QString)));

}
void MenuController::handleSelectedItem(QString itemId) {
    m_desktopBackInterface->HandleSelectedMenuItem(itemId);
}
void MenuController::menuItemInvoked(QString itemId) {
    signalManager->sendItemDesktopBack(itemId);
    qDebug() << itemId;
}
