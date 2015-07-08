#include "desktopitemmanager.h"
#include "global.h"


DesktopItemManager::DesktopItemManager(QObject* parent):QObject(parent){

    m_parentWindow = static_cast<QWidget*>(this->parent());

    QSettings settings;
    settings.beginGroup("Desktop");
    int flag = settings.value("sortFlag", QDir::Name).toInt();
    settings.endGroup();
    m_sortFlag = static_cast<QDir::SortFlag>(flag);
    initConnect();
}

void DesktopItemManager::initComputerItem(){
    QString url = "Computer://";
    int width = gridManager->getItemWidth();
    int height = gridManager->getItemHeight();
    m_pComputerItem  = DesktopItemPointer::create(":/skin/images/QFramer.png", this->tr("Computer"), m_parentWindow);
    m_pComputerItem->setUrl(url);
    m_pComputerItem->resize(width, height);

    GridItemPointer pGridItem = gridManager->getItems().at(0)->at(0);
    QRect rect = pGridItem->getRect();
    QPoint pos = rect.topLeft();

    m_settings.beginGroup("DesktopItems");
    pos = m_settings.value(url, pos).toPoint();
    m_settings.endGroup();

    pGridItem = gridManager->getItemByPos(pos);
    if (!pGridItem.isNull()){
        pGridItem->setDesktopItem(true);
    }
    m_pComputerItem->move(pos);
    m_pItems.insert(url, m_pComputerItem);
    m_list_pItems.append(m_pComputerItem);
}

void DesktopItemManager::initTrashItem(){
    QString url = "Trash://";
    int width = gridManager->getItemWidth();
    int height = gridManager->getItemHeight();
    m_pTrashItem  = DesktopItemPointer::create(":/skin/images/QFramer.png", this->tr("Trash"), m_parentWindow);
    m_pTrashItem->setUrl(url);
    m_pTrashItem->resize(width, height);

    GridItemPointer pGridItem = gridManager->getItems().at(0)->at(1);
    QRect rect = pGridItem->getRect();
    QPoint pos = rect.topLeft();
    m_settings.beginGroup("DesktopItems");
    pos = m_settings.value(url, pos).toPoint();
    m_settings.endGroup();

    pGridItem = gridManager->getItemByPos(pos);
    if (!pGridItem.isNull()){
        pGridItem->setDesktopItem(true);
    }
    m_pTrashItem->move(pos);
    m_pItems.insert(url, m_pTrashItem);
    m_list_pItems.append(m_pTrashItem);
}

void DesktopItemManager::initDesktopFolder(){
    int width = gridManager->getItemWidth();
    int height = gridManager->getItemHeight();
    int row = gridManager->getRowCount();

    QDir desktopDir(desktopLocation);
    QFileInfoList desktopInfoList = desktopDir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot |QDir::Hidden, m_sortFlag);

    int size = desktopInfoList.size();
    m_settings.beginGroup("DesktopItems");
    for (int i = 0; i < size; i++) {
        int _column = (i + 2) /  row;
        int _row = (i + 2) % row;
        QFileInfo fileInfo = desktopInfoList.at(i);
        QString url = fileInfo.absoluteFilePath();
//        m_fileSystemWatcher->addPath(url);
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
            QPoint pos = rect.topLeft();
            pos = m_settings.value(url, pos).toPoint();
            pDesktopItem->move(pos);

            pGridItem = gridManager->getItemByPos(pos);
            if (!pGridItem.isNull()){
                pGridItem->setDesktopItem(true);
            }
        }
    }
    m_settings.endGroup();
}


void DesktopItemManager::initConnect(){
    connect(signalManager, SIGNAL(desktopItemsLoaded(EntryInfoObjList&)), this, SLOT(addItems(EntryInfoObjList&)));
    connect(signalManager, SIGNAL(desktopItemsSaved()), this, SLOT(saveItems()));
    connect(signalManager, SIGNAL(sortedModeChanged(QDir::SortFlag)), this, SLOT(sortedByFlags(QDir::SortFlag)));
    connect(signalManager, SIGNAL(gridOnResorted()), this, SLOT(resort()));
}

void DesktopItemManager::loadDesktopItems(){
    initComputerItem();
    initTrashItem();
//    initDesktopFolder();
}

void DesktopItemManager::addItems(EntryInfoObjList& entryInfoObjList){
    for(int i=0; i< entryInfoObjList.count(); i++){
        addItem(entryInfoObjList.at(i), i);
    }
}

void DesktopItemManager::addItem(EntryInfoObj fileInfo, int index){
    int width = gridManager->getItemWidth();
    int height = gridManager->getItemHeight();
    int row = gridManager->getRowCount();

    m_settings.beginGroup("DesktopItems");

    int _column = (index + 2) /  row;
    int _row = (index + 2) % row;
    QString desktopDisplayName;
    QString displayName = fileInfo.DisplayName;
    QString uri = fileInfo.URI;
    QString url = QUrl(uri).toString();

    if (displayName.startsWith(RichDirPrefix)){
        int l = QString(RichDirPrefix).length();
        desktopDisplayName = displayName.remove(0, l);
    }else{
        desktopDisplayName = displayName;
    }

    DesktopItemPointer  pDesktopItem = DesktopItemPointer::create(":/skin/images/QFramer.png", desktopDisplayName, m_parentWindow);
    pDesktopItem->setUrl(url);
    pDesktopItem->resize(width, height);
    pDesktopItem->show();
    m_pItems.insert(url, pDesktopItem);
    m_list_pItems.append(pDesktopItem);

    GridItemPointer pGridItem = gridManager->getItems().at(_column)->at(_row);
    if (!pGridItem.isNull()){
        QRect rect = pGridItem->getRect();
        QPoint pos = rect.topLeft();
        pos = m_settings.value(url, pos).toPoint();
        pDesktopItem->move(pos);

        pGridItem = gridManager->getItemByPos(pos);
        if (!pGridItem.isNull()){
            pGridItem->setDesktopItem(true);
        }
    }
    m_settings.endGroup();
}


void DesktopItemManager::saveItems(){
    m_settings.beginGroup("DesktopItems");
    foreach (DesktopItemPointer pItem, m_list_pItems) {
        m_settings.setValue(pItem->getUrl(), pItem->pos());
    }
    m_settings.endGroup();
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
    emit signalManager->desktopItemsSaved();
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
        QString url = "file://" + QUrl(fileInfo.absoluteFilePath()).toString();
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
    emit signalManager->desktopItemsSaved();
    m_sortFlag = flag;
}

void DesktopItemManager::resort(){
    sortedByFlags(m_sortFlag);
}

QDir::SortFlag DesktopItemManager::getSortFlag(){
    return m_sortFlag;
}

DesktopItemManager::~DesktopItemManager()
{

}
