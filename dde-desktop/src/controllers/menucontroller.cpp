#include "menucontroller.h"
#include "views/global.h"

MenuController::MenuController(QObject *parent) : QObject(parent)
{
    initConnect();
}

void MenuController::initConnect(){
    connect(signalManager, SIGNAL(contextMenuShowed(DesktopItemPointer,QPoint)), this, SLOT(showMenuByDesktopItem(DesktopItemPointer,QPoint)));
}


void MenuController::showMenuByDesktopItem(DesktopItemPointer pItem, QPoint pos){
    qDebug() << pItem->getUrl() << pos;
}

MenuController::~MenuController()
{

}

