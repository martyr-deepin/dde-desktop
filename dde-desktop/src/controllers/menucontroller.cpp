#include "menucontroller.h"
#include "views/global.h"

MenuController::MenuController(QObject *parent) : QObject(parent)
{
    initConnect();
}

void MenuController::initConnect(){
    connect(signalManager, SIGNAL(contextMenuShowed(DesktopItemPointer,QPoint)),
            this, SLOT(showMenuByDesktopItem(DesktopItemPointer,QPoint)));
    connect(signalManager, SIGNAL(contextMenuShowed(QList<DesktopItemPointer>,DesktopItemPointer,QPoint)),
            this, SLOT(showMenuByDesktopItem(QList<DesktopItemPointer>,DesktopItemPointer,QPoint)));
}


void MenuController::showMenuByDesktopItem(DesktopItemPointer pItem, QPoint pos){
    if (!pItem.isNull()){
        qDebug() << pItem->getUrl() << pos;
    }else{
        qDebug() << "Desktop Menu" << pos;
    }

}

void MenuController::showMenuByDesktopItem(const QList<DesktopItemPointer> &pCheckItems,
                                           const DesktopItemPointer &pItem, QPoint pos){
    qDebug() << pCheckItems << pItem << pos;
}

MenuController::~MenuController()
{

}

