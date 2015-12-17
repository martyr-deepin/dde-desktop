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
    connect(signalManager, SIGNAL(showTextEditMenuBySelectContent(QString,QString,QString,QPoint)),
            this, SLOT(showTexitEditMenuBySelectContent(QString,QString,QString,QPoint)));
}

MenuController::~MenuController()
{
}

QJsonObject MenuController::createMenuItem(int itemId, QString itemText){
    QJsonObject itemObj;
    itemObj["itemId"] = QString::number(itemId);
    itemObj["itemText"] = itemText;
    itemObj["isActive"] = true;
    itemObj["isCheckable"] = false;
    itemObj["checked"] = false;
    itemObj["itemIcon"] = "";
    itemObj["itemIconHover"] = "";
    itemObj["itemIconInactive"] = "";
    itemObj["showCheckMark"] = false;
    QJsonObject subMenuObj;
    subMenuObj["checkableMenu"] = false;
    subMenuObj["singleCheck"] = false;
    subMenuObj["items"] = QJsonArray();
    itemObj["itemSubMenu"] = subMenuObj;
    return itemObj;
}


QJsonObject MenuController::createSeperator(){
    return createMenuItem(-100, "");
}

QString MenuController::JsonToQString(QPoint pos, QString menucontent) {
    QJsonObject menuObj;
    menuObj["x"] = pos.x();
    menuObj["y"] = pos.y();
    menuObj["isDockMenu"] = false;
    menuObj["menuJsonContent"] = menucontent;
    return QString(QJsonDocument(menuObj).toJson());
}

QString MenuController::registerMenu() {
    QDBusPendingReply<QDBusObjectPath> reply = m_menuManagerInterface->RegisterMenu();
    reply.waitForFinished();
    if (!reply.isError()) {
        return reply.value().path();
    } else {
        return "";
    }
}

void MenuController::showTexitEditMenu(QString menuDBusObjectPath, QString menuContent) {
    m_menuInterface = new MenuInterface(MenuManager_service, menuDBusObjectPath, QDBusConnection::sessionBus(), this);
    m_menuInterface->ShowMenu(menuContent);
    connect(m_menuInterface, SIGNAL(ItemInvoked(QString, bool)),this, SLOT(textEditMenuInvoked(QString,bool)));
    connect(m_menuInterface, SIGNAL(MenuUnregistered()), m_menuInterface, SLOT(deleteLater()));
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
        qCritical() << "register menu fail!";
    }
}

void MenuController::showMenuByDesktopItem(DesktopItemPointer pItem, QPoint pos){
    QStringList urls;
    if (!pItem.isNull()) {
        urls = QStringList(pItem->getRawUrl());
    }
    showMenuByUrls(urls, pos);
}


void MenuController::showMenuByDesktopItem(const QList<DesktopItemPointer> &pCheckItems,
                                           const DesktopItemPointer &pItem, QPoint pos){
    qDebug() << pCheckItems << pItem << pos;
    QStringList urls;
    foreach (DesktopItemPointer pItem, pCheckItems) {
        urls.append(pItem->getRawUrl());
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
        qCritical() << "register menu fail!";
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

void MenuController::showMenu(const QString showmenu_path, QString menucontent) {
    m_menuInterface = new MenuInterface(MenuManager_service, showmenu_path, QDBusConnection::sessionBus(), this);
    connect(m_menuInterface, SIGNAL(ItemInvoked(QString, bool)),this, SLOT(menuItemInvoked(QString,bool)));
    connect(m_menuInterface, SIGNAL(MenuUnregistered()), dbusController->getDesktopDaemonInterface(), SLOT(DestroyMenu()));
    connect(m_menuInterface, SIGNAL(MenuUnregistered()), this, SLOT(handleMenuUnregistered()));
    connect(m_menuInterface, SIGNAL(MenuUnregistered()), m_menuInterface, SLOT(deleteLater()));
    QDBusPendingReply<> reply = m_menuInterface->ShowMenu(menucontent);
    reply.waitForFinished();
    if (!reply.isError()) {
        emit signalManager->contextMenuShowed(true);
    } else {
        emit signalManager->contextMenuShowed(false);
    }
}

void MenuController::handleMenuUnregistered(){
    emit signalManager->contextMenuShowed(false);
}

void MenuController::menuItemInvoked(QString itemId, bool flag){
    Q_UNUSED(flag)
    qDebug() << itemId << flag;
    dbusController->getDesktopDaemonInterface()->HandleSelectedMenuItem(itemId);
    emit signalManager->appGounpDetailClosed();
}


QString MenuController::createTextEditMenuContentByContent(QString fullText, QString selectText){
    qDebug() << fullText << selectText;
    QJsonObject cutObj = createMenuItem(0, tr("Cut(_X)"));
    QJsonObject copyObj = createMenuItem(1, tr("Copy(_C)"));
    QJsonObject pasteObj = createMenuItem(2, tr("Paste(_V)"));
    QJsonObject selectAllObj = createMenuItem(3, tr("Select All(_A)"));

    if (selectText.length() == 0 && fullText.length() == 0){
        cutObj["isActive"] = false;
        copyObj["isActive"] = false;
        selectAllObj["isActive"] = false;
    }else if (selectText.length() == fullText.length()){
        selectAllObj["isActive"] = false;
    }

    QJsonArray items;
    items.append(cutObj);
    items.append(copyObj);
    items.append(pasteObj);
    items.append(selectAllObj);

    QJsonObject menuObj;
    menuObj["checkableMenu"] = false;
    menuObj["singleCheck"] = false;
    menuObj["items"] = items;

    return QString(QJsonDocument(menuObj).toJson());
}

void MenuController::showTexitEditMenuBySelectContent(QString url, QString fullText, QString selectText, QPoint pos){
    m_url = url;
    QString menucontent = createTextEditMenuContentByContent(fullText, selectText);
    QString menucontentfinal = JsonToQString(pos, menucontent);
    QString menucreatepath = registerMenu();
    if (menucreatepath.length() > 0){
        showTexitEditMenu(menucreatepath, menucontentfinal);
    }else{
        qCritical() << "register textedit menu fail!";
    }
}

void MenuController::textEditMenuInvoked(QString itemId, bool flag){
    Q_UNUSED(flag)
    int id = itemId.toInt();
    qDebug() << "menuItemInvoked" << itemId << m_url;
    switch (id) {
    case 0:
        emit signalManager->desktopItemNameCuted(m_url);
        break;
    case 1:
        emit signalManager->desktopItemNameCopyed(m_url);
        break;
    case 2:
        emit signalManager->desktopItemNamePasted(m_url);
        break;
    case 3:
        emit signalManager->desktopItemNameSelectAll(m_url);
        break;
    default:
        break;
    }
}
