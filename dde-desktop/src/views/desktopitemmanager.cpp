#include "desktopitemmanager.h"
#include "global.h"


DesktopItemManager::DesktopItemManager(){

}

void DesktopItemManager::loadDesktopItems(){
    int width = gridManager->getItemWidth();
    int height = gridManager->getItemHeight();
    int column = gridManager->getColumnCount() / 2;
    int row = gridManager->getRowCount() / 2;

    for(int i=0; i<column; i++){
        for (int j=0; j< row; j++){
            DesktopItemPointer  desktopItem = DesktopItemPointer::create(":/skin/images/QFramer.png", QString::number(i * row + j));
            qDebug() << desktopItem.data();
            desktopItem->resize(width, height);
            m_desktopItems.append(desktopItem);
            GridItemPointer gridItem = gridManager->getItems().at(i)->at(j);
            if (!gridItem.isNull()){
                QRect rect = gridItem->getRect();
                desktopItem->move(rect.topLeft());
                gridItem->setDesktopItem(true);
            }
        }
    }
}


void DesktopItemManager::changeSizeByGrid(){
    int width = gridManager->getItemWidth();
    int height = gridManager->getItemHeight();
    int column = gridManager->getColumnCount();
    int row = gridManager->getRowCount();

    foreach (DesktopItemPointer pItem, m_desktopItems) {
        pItem->resize(width, height);
        int i = m_desktopItems.indexOf(pItem) / row;
        int j = m_desktopItems.indexOf(pItem) % row;
        GridItemPointer gridItem = gridManager->getItems().at(i)->at(j);
        if (!gridItem.isNull()){
            QRect rect = gridItem->getRect();
            pItem->move(rect.topLeft());
            gridItem->setDesktopItem(true);
        }
    }
}

QList<DesktopItemPointer> DesktopItemManager::getItems(){
    return m_desktopItems;
}

DesktopItemManager::~DesktopItemManager()
{

}
