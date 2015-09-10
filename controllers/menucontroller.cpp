#include "menucontroller.h"
#include "dbusinterface/menumanager_interface.h"
#include "dbusinterface/menu_interface.h"
#include "dbusinterface/desktopdaemon_interface.h"
#include "views/global.h"
#include "dbuscontroller.h"

MenuController::MenuController(QObject *parent) : QObject(parent)
{
    m_menuManagerInterface = new MenumanagerInterface(MenuManager_service, MenuManager_path, QDBusConnection::sessionBus(), this);
    m_menuInterface = NULL;
    initConnect();
}


void MenuController::initConnect(){
    connect(signalManager, SIGNAL(contextMenuShowed(QString,QPoint)),
            this, SLOT(showMenuByDesktopItem(QString,QPoint)));
    connect(signalManager, SIGNAL(contextMenuShowed(QList<DesktopItemPointer>,DesktopItemPointer,QPoint)),
            this, SLOT(showMenuByDesktopItem(QList<DesktopItemPointer>,DesktopItemPointer,QPoint)));
    connect(signalManager, SIGNAL(contextMenuShowed(DesktopItemPointer,QPoint)),
            this, SLOT(showMenuByDesktopItem(DesktopItemPointer,QPoint)));
}

MenuController::~MenuController()
{
}


void MenuController::showMenuByDesktopItem(QString url, QPoint pos){
    QStringList urlList;
    urlList.append(url);
    QString menucontent = createMenuContent(urlList);
    QString menucontentfinal = JsonToQString(pos, menucontent);
    QString menucreatepath = registerMenu();
    if (menucreatepath.length() > 0){
        showMenu(menucreatepath, menucontentfinal);
    }else{
        LOG_ERROR() << "register menu fail!";
    }
}

void MenuController::showMenuByDesktopItem(DesktopItemPointer pItem, QPoint pos){
    QStringList urls;
    if (!pItem.isNull()) {
        urls = QStringList(pItem->getUrl());
    }
    showMenuByUrls(urls, pos);
}


void MenuController::showMenuByDesktopItem(const QList<DesktopItemPointer> &pCheckItems,
                                           const DesktopItemPointer &pItem, QPoint pos){
    LOG_INFO() << pCheckItems << pItem << pos;
    QStringList urls;
    foreach (DesktopItemPointer pItem, pCheckItems) {
        urls.append(pItem->getUrl());
    }
    showMenuByUrls(urls, pos);
}


void MenuController::showMenuByUrls(QStringList urls, QPoint pos){
    QString menucontent = createMenuContent(urls);
    QString menucontentfinal = JsonToQString(pos, menucontent);
    QString menucreatepath = registerMenu();
    if (menucreatepath.length() > 0){
        showMenu(menucreatepath, menucontentfinal);
    }else{
        LOG_ERROR() << "register menu fail!";
    }
}

QString MenuController::createMenuContent(QStringList createmenupath) {
    QDBusPendingReply<QString> menu_content_reply = dbusController->getDesktopDaemonInterface()->GenMenuContent(createmenupath);
    menu_content_reply.waitForFinished();
    if (!menu_content_reply.isError()){
        QString menu_content = menu_content_reply.argumentAt(0).toString();
        return menu_content;
    } else {
        return "";
    }
}

QString MenuController::JsonToQString(QPoint pos, QString menucontent) {
    QJsonObject menuObj;
    menuObj["x"] = pos.x();
    menuObj["y"] = pos.y();
    menuObj["isDockMenu"] = false;
    menuObj["menuJsonContent"] = menucontent;
//    qDebug() << menucontent;
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
    m_menuInterface = new MenuInterface(MenuManager_service, showmenu_path, QDBusConnection::sessionBus(), this);
    m_menuInterface->ShowMenu(menucontent);
    connect(m_menuInterface, SIGNAL(ItemInvoked(QString, bool)),this, SLOT(menuItemInvoked(QString,bool)));
    connect(m_menuInterface, SIGNAL(MenuUnregistered()), dbusController->getDesktopDaemonInterface(), SLOT(DestroyMenu()));
    connect(m_menuInterface, SIGNAL(MenuUnregistered()), m_menuInterface, SLOT(deleteLater()));
}

void MenuController::menuItemInvoked(QString itemId, bool flag){
    Q_UNUSED(flag)
    dbusController->getDesktopDaemonInterface()->HandleSelectedMenuItem(itemId);
    emit signalManager->appGounpDetailClosed();
}
