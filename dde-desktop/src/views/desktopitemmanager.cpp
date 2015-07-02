#include "desktopitemmanager.h"
#include "global.h"


DesktopItemManager::DesktopItemManager(QObject* parent):QObject(parent){

    m_parentWindow = static_cast<QWidget*>(this->parent());
    m_sortFlag = QDir::Name;
    initConnect();
}

void DesktopItemManager::initComputerItem(){
    int width = gridManager->getItemWidth();
    int height = gridManager->getItemHeight();
    m_pComputerItem  = DesktopItemPointer::create(":/skin/images/QFramer.png", this->tr("Computer"), m_parentWindow);
    m_pComputerItem->resize(width, height);
    GridItemPointer pGridItem = gridManager->getItems().at(0)->at(0);
    QRect rect = pGridItem->getRect();
    pGridItem->setDesktopItem(true);
    m_pComputerItem->move(rect.topLeft());
    m_pItems.insert("Computer", m_pComputerItem);
    m_list_pItems.append(m_pComputerItem);
}

void DesktopItemManager::initTrashItem(){
    int width = gridManager->getItemWidth();
    int height = gridManager->getItemHeight();
    m_pTrashItem  = DesktopItemPointer::create(":/skin/images/QFramer.png", this->tr("Trash"), m_parentWindow);
    m_pTrashItem->resize(width, height);
    GridItemPointer pGridItem = gridManager->getItems().at(0)->at(1);
    QRect rect = pGridItem->getRect();
    pGridItem->setDesktopItem(true);
    m_pTrashItem->move(rect.topLeft());
    m_pItems.insert("Trash", m_pTrashItem);
    m_list_pItems.append(m_pTrashItem);
}

void DesktopItemManager::initDesktopFolder(){
    int width = gridManager->getItemWidth();
    int height = gridManager->getItemHeight();
    int row = gridManager->getRowCount();

    QDir desktopDir(desktopLocation);
    QFileInfoList desktopInfoList = desktopDir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot |QDir::Hidden, m_sortFlag);

    int size = desktopInfoList.size();
    for (int i = 0; i < size; i++) {
        int _column = (i + 2) /  row;
        int _row = (i + 2) % row;
        QFileInfo fileInfo = desktopInfoList.at(i);
        QString url = fileInfo.absoluteFilePath();
        QString fileName = fileInfo.fileName();
        QString baseName;
        if (fileName.startsWith(RichDirPrefix)){
            int l = QString(RichDirPrefix).length();
            baseName = fileName.remove(0, l);
        }else{
            baseName = fileInfo.baseName();
        }

        DesktopItemPointer  pDesktopItem = DesktopItemPointer::create(":/skin/images/QFramer.png", baseName, m_parentWindow);
        pDesktopItem->setUrl(url);
        pDesktopItem->resize(width, height);
        m_pItems.insert(url, pDesktopItem);
        m_list_pItems.append(pDesktopItem);
        GridItemPointer pGridItem = gridManager->getItems().at(_column)->at(_row);
        if (!pGridItem.isNull()){
            QRect rect = pGridItem->getRect();
            pDesktopItem->move(rect.topLeft());
            pGridItem->setDesktopItem(true);
        }
    }
}


void DesktopItemManager::initConnect(){
    connect(signalManager, SIGNAL(orderByName()), this, SLOT(sortedByName()));
    connect(signalManager, SIGNAL(orderBySize()), this, SLOT(sortedBySize()));
    connect(signalManager, SIGNAL(orderByType()), this, SLOT(sortedByType()));
    connect(signalManager, SIGNAL(orderByTime()), this, SLOT(sortedByTime()));
    connect(signalManager, SIGNAL(gridOnResorted()), this, SLOT(resort()));
}

void DesktopItemManager::loadDesktopItems(){
    initComputerItem();
    initTrashItem();
    initDesktopFolder();
}


void DesktopItemManager::changeSizeByGrid(){
    int width = gridManager->getItemWidth();
    int height = gridManager->getItemHeight();
    int row = gridManager->getRowCount();

    foreach (DesktopItemPointer pItem, m_list_pItems) {
        pItem->resize(width, height);
        int i = m_list_pItems.indexOf(pItem) / row;
        int j = m_list_pItems.indexOf(pItem) % row;
        GridItemPointer pGridItem = gridManager->getItems().at(i)->at(j);
        if (!pGridItem.isNull()){
            QRect rect = pGridItem->getRect();
            pItem->move(rect.topLeft());
            pGridItem->setDesktopItem(true);
        }
    }
}

QList<DesktopItemPointer> DesktopItemManager::getItems(){
    return m_list_pItems;
}

void DesktopItemManager::sortedByFlags(QDir::SortFlag flag){
    QDir desktopDir(desktopLocation);
    QFileInfoList desktopInfoList = desktopDir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot |QDir::Hidden, flag);
    m_list_pItems.clear();
    m_list_pItems.append(m_pComputerItem);
    m_list_pItems.append(m_pTrashItem);

    int row = gridManager->getRowCount();
    int size = desktopInfoList.size();
    for (int i = 0; i < size; i++) {
        QFileInfo fileInfo = desktopInfoList.at(i);
        QString url = fileInfo.absoluteFilePath();
        DesktopItemPointer  pDesktopItem = m_pItems.value(url);
        m_list_pItems.append(pDesktopItem);
    }

    foreach (DesktopItemPointer pItem, m_list_pItems) {
        int i = m_list_pItems.indexOf(pItem) / row;
        int j = m_list_pItems.indexOf(pItem) % row;
        GridItemPointer gridItem = gridManager->getItems().at(i)->at(j);
        if (!gridItem.isNull()){
            QRect rect = gridItem->getRect();
            pItem->move(rect.topLeft());
        }
    }

    m_sortFlag = flag;
}

void DesktopItemManager::sortedByName(){
    sortedByFlags(QDir::Name);
}

void DesktopItemManager::sortedByTime(){
    sortedByFlags(QDir::Time);
}

void DesktopItemManager::sortedBySize(){
    sortedByFlags(QDir::Size);
}

void DesktopItemManager::sortedByType(){
    sortedByFlags(QDir::Type);
}

void DesktopItemManager::resort(){
    sortedByFlags(m_sortFlag);
}

DesktopItemManager::~DesktopItemManager()
{

}
