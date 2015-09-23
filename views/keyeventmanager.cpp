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
                    static_cast<DesktopFrame*>(parent())->getDesktopItemManager()->getItemByPos(pos);
            if(!nextCheckedDesktopItem.isNull()){
                static_cast<DesktopFrame*>(parent())->unCheckCheckedItems();
                emit lastCheckedDesktopItem->setChecked(false);
                emit nextCheckedDesktopItem->setChecked(true);
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
                    static_cast<DesktopFrame*>(parent())->getDesktopItemManager()->getItemByPos(pos);
            if(!nextCheckedDesktopItem.isNull()){
                static_cast<DesktopFrame*>(parent())->unCheckCheckedItems();
                emit lastCheckedDesktopItem->setChecked(false);
                emit nextCheckedDesktopItem->setChecked(true);
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
                    static_cast<DesktopFrame*>(parent())->getDesktopItemManager()->getItemByPos(pos);
            if(!nextCheckedDesktopItem.isNull()){
                static_cast<DesktopFrame*>(parent())->unCheckCheckedItems();
                emit lastCheckedDesktopItem->setChecked(false);
                emit nextCheckedDesktopItem->setChecked(true);
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
                    static_cast<DesktopFrame*>(parent())->getDesktopItemManager()->getItemByPos(pos);
            if(!nextCheckedDesktopItem.isNull()){
                static_cast<DesktopFrame*>(parent())->unCheckAllItems();
                emit lastCheckedDesktopItem->setChecked(false);
                emit nextCheckedDesktopItem->setChecked(true);
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
                emit lastCheckedDesktopItem->setChecked(true);
                emit static_cast<DesktopFrame*>(parent())->checkedDesktopItemsAdded(lastCheckedDesktopItem);
            }
        }
    }
}

void KeyEventManager::onKeyShiftLeftPressed(){

    DesktopItemPointer lastCheckedDesktopItem;
    if (!static_cast<DesktopFrame*>(parent())->getLastPressedCheckedDesktopItem().isNull()){
        lastCheckedDesktopItem = static_cast<DesktopFrame*>(parent())->getLastCheckedDesktopItem();
        clearMultiCheckedByMouse();
    }else{
        lastCheckedDesktopItem = m_lastCheckedByKeyboard;
    }

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
                    static_cast<DesktopFrame*>(parent())->getDesktopItemManager()->getItemByPos(pos);
            if(!nextCheckedDesktopItem.isNull()){

                if (nextCheckedDesktopItem->isChecked() && index > column){
                    emit nextCheckedDesktopItem->setChecked(false);
                    emit static_cast<DesktopFrame*>(parent())->removeCheckedDesktopItem(nextCheckedDesktopItem);
                    break;
                }else if (!nextCheckedDesktopItem->isChecked() && index < column){
                    emit nextCheckedDesktopItem->setChecked(true);
                    emit static_cast<DesktopFrame*>(parent())->checkedDesktopItemsAdded(nextCheckedDesktopItem);
                    break;
                }
            }
        }
        if (static_cast<DesktopFrame*>(parent())->getCheckedDesktopItems().count() > 1){
            emit static_cast<DesktopFrame*>(parent())->multiCheckedByMouseChanged(false);
        }
    }
}

void KeyEventManager::onKeyShiftRightPressed(){
    DesktopItemPointer lastCheckedDesktopItem;
    if (!static_cast<DesktopFrame*>(parent())->getLastPressedCheckedDesktopItem().isNull()){
        lastCheckedDesktopItem = static_cast<DesktopFrame*>(parent())->getLastCheckedDesktopItem();
    }else{
        lastCheckedDesktopItem = m_lastCheckedByKeyboard;
    }
    clearMultiCheckedByMouse();
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
                    static_cast<DesktopFrame*>(parent())->getDesktopItemManager()->getItemByPos(pos);
            if(!nextCheckedDesktopItem.isNull()){
                if (nextCheckedDesktopItem->isChecked() && index < column){
                    emit nextCheckedDesktopItem->setChecked(false);
                    emit static_cast<DesktopFrame*>(parent())->removeCheckedDesktopItem(nextCheckedDesktopItem);
                    break;
                }
                if (!nextCheckedDesktopItem->isChecked() && index > column){
                    emit nextCheckedDesktopItem->setChecked(true);
                    emit static_cast<DesktopFrame*>(parent())->checkedDesktopItemsAdded(nextCheckedDesktopItem);
                    break;
                }
            }
            index++;
        }
        if (static_cast<DesktopFrame*>(parent())->getCheckedDesktopItems().count() > 1){
            emit static_cast<DesktopFrame*>(parent())->multiCheckedByMouseChanged(false);
        }
    }
}

void KeyEventManager::onKeyShiftUpPressed(){
    clearMultiCheckedByMouse();
    DesktopItemPointer lastCheckedDesktopItem = static_cast<DesktopFrame*>(parent())->getLastCheckedDesktopItem();
    if (!lastCheckedDesktopItem.isNull()){
        QString key = lastCheckedDesktopItem->gridKey();
        GridItemPointer pItem = gridManager->getMapItems().value(key);
        int row = pItem->getRow();
        int column = pItem->getColumn();
        int rowCount = gridManager->getRowCount();
        int columnCount = gridManager->getColumnCount();

        int rowIndex;
        for(rowIndex = rowCount - 1; rowIndex > 0; rowIndex--) {
            bool isRowChecked(true);
            QList<DesktopItemPointer> rowLeftItems;
            QList<DesktopItemPointer> rowRightItems;
            QList<DesktopItemPointer> rowItems;
            rowLeftItems.clear();
            rowRightItems.clear();
            rowItems.clear();
            if (rowIndex < rowCount){
                for (int i = 0; i<= column; i++){
                    QPoint pos = gridManager->getItems().at(i)->at(rowIndex)->getPos();
                    DesktopItemPointer pItem = static_cast<DesktopFrame*>(parent())->getDesktopItemManager()->getItemByPos(pos);
                    if (!pItem.isNull()){
                        if (!rowLeftItems.contains(pItem)){
                            rowLeftItems.append(pItem);
                        }
                    }
                }
            }
            if (rowIndex - 1 >= 0){
                for (int i=column + 1; i < columnCount; i++){
                    QPoint pos = gridManager->getItems().at(i)->at(rowIndex - 1)->getPos();
                    DesktopItemPointer pItem = static_cast<DesktopFrame*>(parent())->getDesktopItemManager()->getItemByPos(pos);
                    if (!pItem.isNull()){
                        if (!rowRightItems.contains(pItem)){
                            rowRightItems.append(pItem);
                        }
                    }
                }
            }

            rowItems = rowLeftItems + rowRightItems;
//            qDebug() << rowIndex << rowLeftItems.count() << rowRightItems.count();
            if (rowItems.count() > 0){
                foreach (DesktopItemPointer _pItem, rowItems) {
                    isRowChecked = isRowChecked && _pItem->isChecked();
                }
            }else{
                continue;
            }

            if (rowIndex >= row + 1 && rowItems.count() > 0 && isRowChecked){
                foreach (DesktopItemPointer _pItem, rowItems) {
                    if (_pItem != lastCheckedDesktopItem){
                        if (_pItem->isChecked()){
                            emit _pItem->setChecked(false);
                            emit static_cast<DesktopFrame*>(parent())->checkedDesktopItemsRemoved(_pItem);
                        }
                    }
                }
                break;
            }
            if (rowIndex < row + 1 && rowItems.count() > 0 && !isRowChecked){
                for (int i= rowItems.count() - 1; i>=0; i--){
                    DesktopItemPointer _pItem = rowItems.at(i);
                    emit _pItem->setChecked(true);
                    emit static_cast<DesktopFrame*>(parent())->checkedDesktopItemsAdded(_pItem);
                }

                if (rowRightItems.count() > 0){
                    m_lastCheckedByKeyboard = rowRightItems.at(0);
                }else{
                    m_lastCheckedByKeyboard = rowLeftItems.at(0);
                }

                break;
            }
        }
        if (static_cast<DesktopFrame*>(parent())->getCheckedDesktopItems().count() > 1){
            emit static_cast<DesktopFrame*>(parent())->multiCheckedByMouseChanged(false);
        }
    }

//    if (!m_lastCheckedByKeyboard.isNull()){
//        qDebug() << m_lastCheckedByKeyboard->getDesktopName();
//    }
}

void KeyEventManager::onKeyShiftDownPressed(){
    clearMultiCheckedByMouse();
    DesktopItemPointer lastCheckedDesktopItem = static_cast<DesktopFrame*>(parent())->getLastCheckedDesktopItem();
    if (!lastCheckedDesktopItem.isNull()){
        QString key = lastCheckedDesktopItem->gridKey();
        GridItemPointer pItem = gridManager->getMapItems().value(key);
        int row = pItem->getRow();
        int column = pItem->getColumn();
        int rowCount = gridManager->getRowCount();
        int columnCount = gridManager->getColumnCount();

        int rowIndex;
        for(rowIndex = 0; rowIndex < rowCount - 1; rowIndex++) {
            bool isRowChecked(true);
            QList<DesktopItemPointer> rowLeftItems;
            QList<DesktopItemPointer> rowRightItems;
            QList<DesktopItemPointer> rowItems;
            rowLeftItems.clear();
            rowRightItems.clear();
            rowItems.clear();
            if (rowIndex >= 0){
                for (int i = column + 1; i< columnCount; i++){
                    QPoint pos = gridManager->getItems().at(i)->at(rowIndex)->getPos();
                    DesktopItemPointer pItem = static_cast<DesktopFrame*>(parent())->getDesktopItemManager()->getItemByPos(pos);
                    if (!pItem.isNull()){
                        if (!rowRightItems.contains(pItem)){
                            rowRightItems.append(pItem);
                        }
                    }
                }
            }

            for (int i=0; i <= column; i++){
                QPoint pos = gridManager->getItems().at(i)->at(rowIndex + 1)->getPos();
                DesktopItemPointer pItem = static_cast<DesktopFrame*>(parent())->getDesktopItemManager()->getItemByPos(pos);
                if (!pItem.isNull()){
                    if (!rowLeftItems.contains(pItem)){
                        rowLeftItems.append(pItem);
                    }
                }
            }

            rowItems = rowRightItems + rowLeftItems;
            if (rowItems.count() > 0){
                foreach (DesktopItemPointer _pItem, rowItems) {
                    isRowChecked = isRowChecked && _pItem->isChecked();
                }
            }else{
                continue;
            }

            if (rowIndex < row && rowItems.count() > 0 && isRowChecked){
                foreach (DesktopItemPointer _pItem, rowItems) {
                    if (_pItem != lastCheckedDesktopItem){
                        emit _pItem->setChecked(false);
                        emit static_cast<DesktopFrame*>(parent())->checkedDesktopItemsRemoved(_pItem);
                    }
                }
                break;
            }
            if (rowIndex >= row && rowItems.count() > 0 && !isRowChecked){
                foreach (DesktopItemPointer _pItem, rowItems) {
                    emit _pItem->setChecked(true);
                    emit static_cast<DesktopFrame*>(parent())->checkedDesktopItemsAdded(_pItem);
                }

                if (rowLeftItems.count() > 0){
                    m_lastCheckedByKeyboard = rowLeftItems.last();
                }else{
                    m_lastCheckedByKeyboard = rowRightItems.last();
                }

                break;
            }
        }
        if (static_cast<DesktopFrame*>(parent())->getCheckedDesktopItems().count() > 1){
            emit static_cast<DesktopFrame*>(parent())->multiCheckedByMouseChanged(false);
        }
    }

//    if (!m_lastCheckedByKeyboard.isNull()){
//        qDebug() << m_lastCheckedByKeyboard->getDesktopName();
//    }
}

KeyEventManager::~KeyEventManager()
{

}

