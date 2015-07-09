#include "keyeventmanager.h"
#include "desktopframe.h"
#include "desktopitem.h"
#include "global.h"

KeyEventManager::KeyEventManager(QWidget *parent) : QObject(parent)
{
    initConnect();
}


void KeyEventManager::initConnect(){
    connect(signalManager, SIGNAL(keyUpPressed()), this, SLOT(onKeyUpPressed()));
    connect(signalManager, SIGNAL(keyDownPressed()), this, SLOT(onKeyDownPressed()));
    connect(signalManager, SIGNAL(keyLeftPressed()), this, SLOT(onKeyLeftPressed()));
    connect(signalManager, SIGNAL(keyRightPressed()), this, SLOT(onKeyRightPressed()));

    connect(signalManager, SIGNAL(keyShiftLeftPressed()), this, SLOT(onKeyShiftLeftPressed()));
    connect(signalManager, SIGNAL(keyShiftRightPressed()), this, SLOT(onKeyShiftRightPressed()));
    connect(signalManager, SIGNAL(keyShiftUpPressed()), this, SLOT(onKeyShiftUpPressed()));
    connect(signalManager, SIGNAL(keyShiftDownPressed()), this, SLOT(onKeyShiftDownPressed()));
}

void KeyEventManager::onKeyUpPressed(){
    DesktopItemPointer lastCheckedDesktopItem = static_cast<DesktopFrame*>(parent())->getLastCheckedDesktopItem();
    if (!lastCheckedDesktopItem.isNull()){
        QString key = lastCheckedDesktopItem->gridKey();
        GridItemPointer pItem = gridManager->getMapItems().value(key);
        int row = pItem->getRow();
        int column = pItem->getColumn();
        while (row > 0){
            QPoint pos = gridManager->getItems().at(column)->at(row - 1)->getPos();
            DesktopItemPointer nextCheckedDesktopItem =
                    static_cast<DesktopFrame*>(parent())->getTopDesktopItemManager()->getItemByPos(pos);
            if(!nextCheckedDesktopItem.isNull()){
                static_cast<DesktopFrame*>(parent())->unCheckAllItems();
                emit lastCheckedDesktopItem->checkedChanged(false);
                emit nextCheckedDesktopItem->checkedChanged(true);
                emit static_cast<DesktopFrame*>(parent())->lastCheckedDesktopItemChanged(nextCheckedDesktopItem);
                break;
            }
            row--;
        }
    }
}

void KeyEventManager::onKeyDownPressed(){
    DesktopItemPointer lastCheckedDesktopItem = static_cast<DesktopFrame*>(parent())->getLastCheckedDesktopItem();
    if (!lastCheckedDesktopItem.isNull()){
        QString key = lastCheckedDesktopItem->gridKey();
        GridItemPointer pItem = gridManager->getMapItems().value(key);
        int row = pItem->getRow();
        int column = pItem->getColumn();
        int rowCount = gridManager->getRowCount();
        while (row < rowCount - 1){
            QPoint pos = gridManager->getItems().at(column)->at(row + 1)->getPos();
            DesktopItemPointer nextCheckedDesktopItem =
                    static_cast<DesktopFrame*>(parent())->getTopDesktopItemManager()->getItemByPos(pos);
            if(!nextCheckedDesktopItem.isNull()){
                static_cast<DesktopFrame*>(parent())->unCheckAllItems();
                emit lastCheckedDesktopItem->checkedChanged(false);
                emit nextCheckedDesktopItem->checkedChanged(true);
                emit static_cast<DesktopFrame*>(parent())->lastCheckedDesktopItemChanged(nextCheckedDesktopItem);
                break;
            }
            row++;
        }
    }
}

void KeyEventManager::onKeyLeftPressed(){
    DesktopItemPointer lastCheckedDesktopItem = static_cast<DesktopFrame*>(parent())->getLastCheckedDesktopItem();
    if (!lastCheckedDesktopItem.isNull()){
        QString key = lastCheckedDesktopItem->gridKey();
        GridItemPointer pItem = gridManager->getMapItems().value(key);
        int row = pItem->getRow();
        int column = pItem->getColumn();
        while (column > 0){
            QPoint pos = gridManager->getItems().at(column - 1)->at(row)->getPos();
            DesktopItemPointer nextCheckedDesktopItem =
                    static_cast<DesktopFrame*>(parent())->getTopDesktopItemManager()->getItemByPos(pos);
            if(!nextCheckedDesktopItem.isNull()){
                static_cast<DesktopFrame*>(parent())->unCheckAllItems();
                emit lastCheckedDesktopItem->checkedChanged(false);
                emit nextCheckedDesktopItem->checkedChanged(true);
                emit static_cast<DesktopFrame*>(parent())->lastCheckedDesktopItemChanged(nextCheckedDesktopItem);
                break;
            }
            column--;
        }
    }
}

void KeyEventManager::onKeyRightPressed(){
    DesktopItemPointer lastCheckedDesktopItem = static_cast<DesktopFrame*>(parent())->getLastCheckedDesktopItem();
    if (!lastCheckedDesktopItem.isNull()){
        QString key = lastCheckedDesktopItem->gridKey();
        GridItemPointer pItem = gridManager->getMapItems().value(key);
        int row = pItem->getRow();
        int column = pItem->getColumn();
        int columnCount = gridManager->getColumnCount();
        while (column < columnCount - 1){
            QPoint pos = gridManager->getItems().at(column + 1)->at(row)->getPos();
            DesktopItemPointer nextCheckedDesktopItem =
                    static_cast<DesktopFrame*>(parent())->getTopDesktopItemManager()->getItemByPos(pos);
            if(!nextCheckedDesktopItem.isNull()){
                static_cast<DesktopFrame*>(parent())->unCheckAllItems();
                emit lastCheckedDesktopItem->checkedChanged(false);
                emit nextCheckedDesktopItem->checkedChanged(true);
                emit static_cast<DesktopFrame*>(parent())->lastCheckedDesktopItemChanged(nextCheckedDesktopItem);
                break;
            }
            column++;
        }
    }
}

void KeyEventManager::clearMultiCheckedByMouse(){
    DesktopItemPointer lastCheckedDesktopItem = static_cast<DesktopFrame*>(parent())->getLastCheckedDesktopItem();
    if (static_cast<DesktopFrame*>(parent())->isMultiCheckedByMouse()){
        if (static_cast<DesktopFrame*>(parent())->getCheckedDesktopItems().count() > 1){
            static_cast<DesktopFrame*>(parent())->unCheckCheckedItems();
            if (!lastCheckedDesktopItem.isNull()){
                emit lastCheckedDesktopItem->checkedChanged(true);
                emit static_cast<DesktopFrame*>(parent())->checkedDesktopItemsAdded(lastCheckedDesktopItem);
            }
        }
    }
}

void KeyEventManager::onKeyShiftLeftPressed(){
    clearMultiCheckedByMouse();
    DesktopItemPointer lastCheckedDesktopItem = static_cast<DesktopFrame*>(parent())->getLastCheckedDesktopItem();
    if (!lastCheckedDesktopItem.isNull()){
        QString key = lastCheckedDesktopItem->gridKey();
        GridItemPointer pItem = gridManager->getMapItems().value(key);
        int row = pItem->getRow();
        int column = pItem->getColumn();
        int columnCount = gridManager->getColumnCount();

        int index=columnCount;
        while (index > 0) {
            index = index - 1;
            QPoint pos = gridManager->getItems().at(index)->at(row)->getPos();
            DesktopItemPointer nextCheckedDesktopItem =
                    static_cast<DesktopFrame*>(parent())->getTopDesktopItemManager()->getItemByPos(pos);
            if(!nextCheckedDesktopItem.isNull()){
                if (!nextCheckedDesktopItem->isChecked() && index > column){

                }else if (!nextCheckedDesktopItem->isChecked() && index < column){
                    emit nextCheckedDesktopItem->checkedChanged(true);
                    emit static_cast<DesktopFrame*>(parent())->checkedDesktopItemsAdded(nextCheckedDesktopItem);
                    break;
                }else if (nextCheckedDesktopItem->isChecked() && index > column){
                    emit nextCheckedDesktopItem->checkedChanged(false);
                    emit static_cast<DesktopFrame*>(parent())->removeCheckedDesktopItem(nextCheckedDesktopItem);
                    break;
                }
            }
        }
    }

    if (static_cast<DesktopFrame*>(parent())->getCheckedDesktopItems().count() > 1){
        emit static_cast<DesktopFrame*>(parent())->multiCheckedByMouseChanged(false);
    }

}

void KeyEventManager::onKeyShiftRightPressed(){
    clearMultiCheckedByMouse();
    DesktopItemPointer lastCheckedDesktopItem = static_cast<DesktopFrame*>(parent())->getLastCheckedDesktopItem();
    if (!lastCheckedDesktopItem.isNull()){
        QString key = lastCheckedDesktopItem->gridKey();
        GridItemPointer pItem = gridManager->getMapItems().value(key);
        int row = pItem->getRow();
        int column = pItem->getColumn();
        int columnCount = gridManager->getColumnCount();
        int index = 0;
        while ( index < columnCount) {
            QPoint pos = gridManager->getItems().at(index)->at(row)->getPos();
            DesktopItemPointer nextCheckedDesktopItem =
                    static_cast<DesktopFrame*>(parent())->getTopDesktopItemManager()->getItemByPos(pos);
            if(!nextCheckedDesktopItem.isNull()){
                if (nextCheckedDesktopItem->isChecked() && index < column){
                    emit nextCheckedDesktopItem->checkedChanged(false);
                    emit static_cast<DesktopFrame*>(parent())->removeCheckedDesktopItem(nextCheckedDesktopItem);
                    break;
                }
                if (!nextCheckedDesktopItem->isChecked() && index > column){
                    emit nextCheckedDesktopItem->checkedChanged(true);
                    emit static_cast<DesktopFrame*>(parent())->checkedDesktopItemsAdded(nextCheckedDesktopItem);
                    break;
                }
            }
            index++;
        }
    }
    if (static_cast<DesktopFrame*>(parent())->getCheckedDesktopItems().count() > 1){
        emit static_cast<DesktopFrame*>(parent())->multiCheckedByMouseChanged(false);
    }
}

void KeyEventManager::onKeyShiftUpPressed(){
    clearMultiCheckedByMouse();
    DesktopItemPointer lastCheckedDesktopItem = static_cast<DesktopFrame*>(parent())->getLastCheckedDesktopItem();

}

void KeyEventManager::onKeyShiftDownPressed(){
    clearMultiCheckedByMouse();
    DesktopItemPointer lastCheckedDesktopItem = static_cast<DesktopFrame*>(parent())->getLastCheckedDesktopItem();

}

KeyEventManager::~KeyEventManager()
{

}

