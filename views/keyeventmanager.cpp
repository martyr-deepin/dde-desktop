#include "desktopitemmanager.h"
#include "keyeventmanager.h"
#include "desktopframe.h"
#include "desktopfwd.h"
#include "global.h"

KeyEventManager::KeyEventManager(QWidget *parent) : QObject(parent)
{
    m_lastCheckedByKeyboard = DesktopItemPointer();
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

    connect(signalManager, SIGNAL(keyHomePressed()), this, SLOT(onKeyHomePressed()));
    connect(signalManager, SIGNAL(keyEndPressed()), this, SLOT(onKeyEndPressed()));

    connect(signalManager, SIGNAL(keyShiftHomePressed()), this, SLOT(onKeyShiftHomePressed()));
    connect(signalManager, SIGNAL(keyShiftEndPressed()), this, SLOT(onKeyShiftEndPressed()));
}

void KeyEventManager::onKeyUpPressed(bool isMutilChecked){
    DesktopItemPointer lastCheckedDesktopItem = static_cast<DesktopFrame*>(parent())->getLastCheckedDesktopItem();
    if (!lastCheckedDesktopItem.isNull()){
        if (!lastCheckedDesktopItem->isChecked()){
            static_cast<DesktopFrame*>(parent())->lastCheckedDesktopItemChanged(lastCheckedDesktopItem);
            return;
        }
        QString key = lastCheckedDesktopItem->gridKey();
        GridItemPointer pItem = gridManager->getMapItems().value(key);
        int row = pItem->getRow();
        int column = pItem->getColumn();
        while (row > 0){
            QPoint pos = gridManager->getItems().at(column)->at(row - 1)->getPos();
            DesktopItemPointer nextCheckedDesktopItem =
                    static_cast<DesktopFrame*>(parent())->getDesktopItemManager()->getItemByPos(pos);
            if(!nextCheckedDesktopItem.isNull()){
                if (!isMutilChecked){
                    static_cast<DesktopFrame*>(parent())->unCheckCheckedItems();
                    lastCheckedDesktopItem->setChecked(false);
                }
                emit static_cast<DesktopFrame*>(parent())->lastCheckedDesktopItemChanged(nextCheckedDesktopItem);
                break;
            }
            row--;
        }
    }else{
        checkFirstDesktopItem();
    }
}

void KeyEventManager::onKeyDownPressed(bool isMutilChecked){
    DesktopItemPointer lastCheckedDesktopItem = static_cast<DesktopFrame*>(parent())->getLastCheckedDesktopItem();
    if (!lastCheckedDesktopItem.isNull()){
        if (!lastCheckedDesktopItem->isChecked()){
            static_cast<DesktopFrame*>(parent())->lastCheckedDesktopItemChanged(lastCheckedDesktopItem);
            return;
        }
        QString key = lastCheckedDesktopItem->gridKey();
        GridItemPointer pItem = gridManager->getMapItems().value(key);
        int row = pItem->getRow();
        int column = pItem->getColumn();
        int rowCount = gridManager->getRowCount();
        while (row < rowCount - 1){
            QPoint pos = gridManager->getItems().at(column)->at(row + 1)->getPos();
            DesktopItemPointer nextCheckedDesktopItem =
                    static_cast<DesktopFrame*>(parent())->getDesktopItemManager()->getItemByPos(pos);
            if(!nextCheckedDesktopItem.isNull()){
                if (!isMutilChecked){
                    static_cast<DesktopFrame*>(parent())->unCheckCheckedItems();
                    lastCheckedDesktopItem->setChecked(false);
                }
                emit static_cast<DesktopFrame*>(parent())->lastCheckedDesktopItemChanged(nextCheckedDesktopItem);
                break;
            }
            row++;
        }
    }else{
        checkFirstDesktopItem();
    }
}

void KeyEventManager::onKeyLeftPressed(bool isMutilChecked){
    DesktopItemPointer lastCheckedDesktopItem = static_cast<DesktopFrame*>(parent())->getLastCheckedDesktopItem();
    if (!lastCheckedDesktopItem.isNull()){
        if (!lastCheckedDesktopItem->isChecked()){
            static_cast<DesktopFrame*>(parent())->lastCheckedDesktopItemChanged(lastCheckedDesktopItem);
            return;
        }
        QString key = lastCheckedDesktopItem->gridKey();
        GridItemPointer pItem = gridManager->getMapItems().value(key);
        int row = pItem->getRow();
        int column = pItem->getColumn();
        while (column > 0){
            QPoint pos = gridManager->getItems().at(column - 1)->at(row)->getPos();
            DesktopItemPointer nextCheckedDesktopItem =
                    static_cast<DesktopFrame*>(parent())->getDesktopItemManager()->getItemByPos(pos);
            if(!nextCheckedDesktopItem.isNull()){
                if (!isMutilChecked){
                    static_cast<DesktopFrame*>(parent())->unCheckCheckedItems();
                    lastCheckedDesktopItem->setChecked(false);
                }
                emit static_cast<DesktopFrame*>(parent())->lastCheckedDesktopItemChanged(nextCheckedDesktopItem);
                break;
            }
            column--;
        }
    }else{
        checkFirstDesktopItem();
    }
}

void KeyEventManager::onKeyRightPressed(bool isMutilChecked){
    DesktopItemPointer lastCheckedDesktopItem = static_cast<DesktopFrame*>(parent())->getLastCheckedDesktopItem();
    if (!lastCheckedDesktopItem.isNull()){
        if (!lastCheckedDesktopItem->isChecked()){
            static_cast<DesktopFrame*>(parent())->lastCheckedDesktopItemChanged(lastCheckedDesktopItem);
            return;
        }
        QString key = lastCheckedDesktopItem->gridKey();
        GridItemPointer pItem = gridManager->getMapItems().value(key);
        int row = pItem->getRow();
        int column = pItem->getColumn();
        int columnCount = gridManager->getColumnCount();
        while (column < columnCount - 1){
            QPoint pos = gridManager->getItems().at(column + 1)->at(row)->getPos();
            DesktopItemPointer nextCheckedDesktopItem =
                    static_cast<DesktopFrame*>(parent())->getDesktopItemManager()->getItemByPos(pos);
            if(!nextCheckedDesktopItem.isNull()){
                if (!isMutilChecked){
                    static_cast<DesktopFrame*>(parent())->unCheckAllItems();
                    lastCheckedDesktopItem->setChecked(false);
                }
                emit static_cast<DesktopFrame*>(parent())->lastCheckedDesktopItemChanged(nextCheckedDesktopItem);
                break;
            }
            column++;
        }
    }else{
        checkFirstDesktopItem();
    }
}

void KeyEventManager::clearMultiCheckedByMouse(){
    DesktopItemPointer lastCheckedDesktopItem = static_cast<DesktopFrame*>(parent())->getLastCheckedDesktopItem();
    if (static_cast<DesktopFrame*>(parent())->isMultiCheckedByMouse()){
        if (static_cast<DesktopFrame*>(parent())->getCheckedDesktopItems().count() > 1){
            static_cast<DesktopFrame*>(parent())->unCheckCheckedItems();
            if (!lastCheckedDesktopItem.isNull()){
                emit lastCheckedDesktopItem->setChecked(true);
                emit static_cast<DesktopFrame*>(parent())->checkedDesktopItemsAdded(lastCheckedDesktopItem);
            }
        }
    }
}

void KeyEventManager::onKeyShiftLeftPressed(){
    onKeyLeftPressed(true);
}

void KeyEventManager::onKeyShiftRightPressed(){
    onKeyRightPressed(true);
}

void KeyEventManager::onKeyShiftUpPressed(){
    onKeyUpPressed(true);
}

void KeyEventManager::onKeyShiftDownPressed(){
    onKeyDownPressed(true);
}

void KeyEventManager::onKeyHomePressed(){
    checkFirstDesktopItem();
}

void KeyEventManager::onKeyEndPressed(){
    checkLastDesktopItem();
}

void KeyEventManager::onKeyShiftHomePressed(){
    DesktopItemPointer lastCheckedDesktopItem = static_cast<DesktopFrame*>(parent())->getLastCheckedDesktopItem();
    if (!lastCheckedDesktopItem.isNull()){
        if (!lastCheckedDesktopItem->isChecked()){
            static_cast<DesktopFrame*>(parent())->lastCheckedDesktopItemChanged(lastCheckedDesktopItem);
            return;
        }
        int x = lastCheckedDesktopItem->pos().x();
        int y = lastCheckedDesktopItem->pos().y();
        QSharedPointer<DesktopItemManager> manager = static_cast<DesktopFrame*>(parent())->getDesktopItemManager();
        foreach (DesktopItemPointer pItem, manager->getItems()) {
            if ((pItem->x() < x) || (pItem->x() == x && pItem->y() >= y)){
                qDebug() << pItem;
                emit static_cast<DesktopFrame*>(parent())->lastCheckedDesktopItemChanged(pItem);
            }
        }
    }else{
        checkFirstDesktopItem();
    }
}

void KeyEventManager::onKeyShiftEndPressed(){
    DesktopItemPointer lastCheckedDesktopItem = static_cast<DesktopFrame*>(parent())->getLastCheckedDesktopItem();
    if (!lastCheckedDesktopItem.isNull()){
        if (!lastCheckedDesktopItem->isChecked()){
            static_cast<DesktopFrame*>(parent())->lastCheckedDesktopItemChanged(lastCheckedDesktopItem);
            return;
        }

        int x = lastCheckedDesktopItem->pos().x();
        int y = lastCheckedDesktopItem->pos().y();
        QSharedPointer<DesktopItemManager> manager = static_cast<DesktopFrame*>(parent())->getDesktopItemManager();
        foreach (DesktopItemPointer pItem, manager->getItems()) {
            if ((pItem->x() > x) || (pItem->x() == x && pItem->y() <= y)){
                qDebug() << pItem;
                emit static_cast<DesktopFrame*>(parent())->lastCheckedDesktopItemChanged(pItem);
            }
        }
    }else{
        checkFirstDesktopItem();
    }
}

void KeyEventManager::checkFirstDesktopItem(){
    static_cast<DesktopFrame*>(parent())->unCheckAllItems();
    int rowCount = gridManager->getRowCount();
    int columnCount = gridManager->getColumnCount();
    for (int column=0; column < columnCount; column++){
        for (int row=0; row < rowCount; row++){
            GridItemPointer pGridItem = gridManager->getItems().at(column)->at(row);
            if (!pGridItem.isNull()){
                if (pGridItem->hasDesktopItem()){
                    QPoint pos = pGridItem->getPos();
                    DesktopItemPointer pItem =
                            static_cast<DesktopFrame*>(parent())->getDesktopItemManager()->getItemByPos(pos);
                    if (!pItem.isNull()){
                        emit static_cast<DesktopFrame*>(parent())->lastCheckedDesktopItemChanged(pItem);
                    }
                    return;
                }
            }
        }
    }
}

void KeyEventManager::checkLastDesktopItem(){
    static_cast<DesktopFrame*>(parent())->unCheckAllItems();
    int rowCount = gridManager->getRowCount();
    int columnCount = gridManager->getColumnCount();
    for (int column=columnCount - 1; column >= 0; column--){
        for (int row=rowCount -1; row >= 0; row--){
            GridItemPointer pGridItem = gridManager->getItems().at(column)->at(row);
            if (!pGridItem.isNull()){
                qDebug() << column << " " << row;
                if (pGridItem->hasDesktopItem()){
                    QPoint pos = pGridItem->getPos();
                    DesktopItemPointer pItem =
                            static_cast<DesktopFrame*>(parent())->getDesktopItemManager()->getItemByPos(pos);
                    if (!pItem.isNull()){
                        emit static_cast<DesktopFrame*>(parent())->lastCheckedDesktopItemChanged(pItem);
                    }
                    return;
                }
            }
        }
    }
}

KeyEventManager::~KeyEventManager()
{

}

