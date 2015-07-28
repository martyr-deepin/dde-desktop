#include "desktopitemmanager.h"
#include "appgroupbox.h"
#include "global.h"
#include "controllers/dbuscontroller.h"
#include "desktopframe.h"

DesktopItemManager::DesktopItemManager(QObject* parent):QObject(parent){

    m_parentWindow = static_cast<QWidget*>(this->parent());

    QSettings settings;
    settings.beginGroup("Desktop");
    int flag = settings.value("sortFlag", QDir::Name).toInt();
    settings.endGroup();
    m_sortFlag = static_cast<QDir::SortFlag>(flag);

    m_shoudbeMovedItem = DesktopItemPointer();
    initConnect();
}

void DesktopItemManager::initComputerItem(){
    QString url = ComputerUrl;
    int width = gridManager->getItemWidth();
    int height = gridManager->getItemHeight();
    m_pComputerItem  = DesktopItemPointer::create(defaut_icon, this->tr("Computer"), m_parentWindow);
    m_pComputerItem->setUrl(url);
    m_pComputerItem->resize(width, height);
    m_pComputerItem->getDesktopItemInfo().URI = url;

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
    QString url = TrashUrl;
    int width = gridManager->getItemWidth();
    int height = gridManager->getItemHeight();
    m_pTrashItem  = DesktopItemPointer::create(defaut_icon, this->tr("Trash"), m_parentWindow);
    m_pTrashItem->setUrl(url);
    m_pTrashItem->resize(width, height);
    m_pTrashItem->getDesktopItemInfo().URI = url;

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


void DesktopItemManager::initConnect(){
    connect(signalManager, SIGNAL(desktopItemsSaved()), this, SLOT(saveItems()));
    connect(signalManager, SIGNAL(desktoItemIconUpdated(QString,QString,uint)),
            this, SLOT(updateDesktopItemIcon(QString,QString,uint)));
    connect(signalManager, SIGNAL(sortedModeChanged(QDir::SortFlag)),
            this, SLOT(sortedByFlags(QDir::SortFlag)));
    connect(signalManager, SIGNAL(sortByKey(QString)),
            this, SLOT(sortedByKey(QString)));

    connect(signalManager, SIGNAL(gridOnResorted()), this, SLOT(resort()));
    connect(signalManager, SIGNAL(desktopItemsChanged(DesktopItemInfoMap&)),
            this, SLOT(addItems(DesktopItemInfoMap&)));
    connect(signalManager, SIGNAL(appGounpItemsChanged(QString,DesktopItemInfoMap&)),
            this, SLOT(updateAppGounpItem(QString,DesktopItemInfoMap&)));
    connect(signalManager, SIGNAL(appGounpDetailShowed(DesktopItemPointer&,QPoint)),
            this, SLOT(showAppGroupDetail(DesktopItemPointer&,QPoint)));
    connect(signalManager, SIGNAL(appGounpDetailClosed(QPoint)), this, SLOT(closeAppGroupDetail(QPoint)));
    connect(signalManager, SIGNAL(appGounpDetailClosed()), this, SLOT(closeAppGroupDetail()));


    connect(signalManager, SIGNAL(itemCreated(DesktopItemInfo)),
            this, SLOT(addItem(DesktopItemInfo)));
    connect(signalManager, SIGNAL(itemDeleted(QString)),
            this, SLOT(deleteItem(QString)));

    connect(signalManager, SIGNAL(itemShoudBeMoved(QString)),
            this, SLOT(setShoudBeMovedItemByUrl(QString)));
    connect(signalManager, SIGNAL(itemMoved(DesktopItemInfo&)),
            this, SLOT(renameDesktopItem(DesktopItemInfo&)));

    connect(signalManager, SIGNAL(filesCuted(QStringList)),
            this, SLOT(cutItems(QStringList)));
    connect(signalManager, SIGNAL(cancelFilesCuted(QStringList)),
            this, SLOT(cancelCutedItems(QStringList)));
}

void DesktopItemManager::loadDesktopItems(){
    initComputerItem();
    initTrashItem();
}

QString DesktopItemManager::decodeUrl(QString url){
    return QUrl(url).toString();
}

QString DesktopItemManager::getDesktopDisplayName(const DesktopItemInfo &desktopItemInfo){
    QString desktopDisplayName;
    QString displayName = desktopItemInfo.DisplayName;
    if (displayName.startsWith(RichDirPrefix)){
        int l = QString(RichDirPrefix).length();
        desktopDisplayName = displayName.remove(0, l);
    }else{
        desktopDisplayName = displayName;
    }
    return desktopDisplayName;
}

DesktopItemPointer DesktopItemManager::getShoudBeMovedItem(){
    return m_shoudbeMovedItem;
}

void DesktopItemManager::setShoudBeMovedItem(DesktopItemPointer pItem){
    m_shoudbeMovedItem = pItem;
}

void DesktopItemManager::setShoudBeMovedItemByUrl(QString url){
    QString key = decodeUrl(url);
    if (m_pItems.contains(key)){
        setShoudBeMovedItem(m_pItems.value(key));
    }else{
        setShoudBeMovedItem(DesktopItemPointer());
    }
}

void DesktopItemManager::addItems(DesktopItemInfoMap &desktopInfoMap){
    for(int i=0; i< desktopInfoMap.values().count(); i++){
        addItem(desktopInfoMap.values().at(i), i);
    }
}

void DesktopItemManager::updateAppGounpItem(QString group_url, DesktopItemInfoMap &appItemInfos){
    QString key = decodeUrl(group_url);
    if (m_pItems.contains(key)){
        m_pItems.value(key)->setAppGroupItems(appItemInfos);
        updateAppGroupDetail(m_pItems.value(key));
        if (static_cast<DesktopFrame*>(this->parent())->getAppGroupDestinationPos() != QPoint(-1, -1)){
            m_pItems.value(key)->move(static_cast<DesktopFrame*>(this->parent())->getAppGroupDestinationPos());
            static_cast<DesktopFrame*>(this->parent())->setAppGroupDestinationPos(QPoint(-1, -1));
        }
    }
}

DesktopItemPointer DesktopItemManager::createItem(const DesktopItemInfo &fileInfo){
    int width = gridManager->getItemWidth();
    int height = gridManager->getItemHeight();

    QString desktopDisplayName = getDesktopDisplayName(fileInfo);
    QString displayName = fileInfo.DisplayName;
    QString uri = fileInfo.URI;
    QString url = decodeUrl(uri);
    QString icon = fileInfo.Icon;

    DesktopItemPointer  pDesktopItem = DesktopItemPointer::create(defaut_icon, desktopDisplayName, m_parentWindow);
    pDesktopItem->setUrl(url);
    pDesktopItem->setDesktopIcon(icon);
    pDesktopItem->setDesktopItemInfo(fileInfo);
    pDesktopItem->resize(width, height);
    pDesktopItem->show();
    return pDesktopItem;
}

void DesktopItemManager::addItem(const DesktopItemInfo& fileInfo, int index){
    DesktopItemPointer pDesktopItem = createItem(fileInfo);
    m_pItems.insert(pDesktopItem->getUrl(), pDesktopItem);
    m_list_pItems.append(pDesktopItem);
    checkPageCount();
    if (!pDesktopItem.isNull()){
        int row = gridManager->getRowCount();
        m_settings.beginGroup("DesktopItems");
        int _column = (index + 2) /  row;
        int _row = (index + 2) % row;
        GridItemPointer pGridItem = gridManager->getItems().at(_column)->at(_row);
        if (!pGridItem.isNull()){
            QRect rect = pGridItem->getRect();
            QPoint pos = rect.topLeft();
            pos = m_settings.value(pDesktopItem->getUrl(), pos).toPoint();
            pDesktopItem->move(pos);

            pGridItem = gridManager->getItemByPos(pos);
            if (!pGridItem.isNull()){
                pGridItem->setDesktopItem(true);
            }
        }
        m_settings.endGroup();
    }
}


void DesktopItemManager::addItem(const DesktopItemInfo& fileInfo){
    DesktopItemPointer pDesktopItem = createItem(fileInfo);
    m_pItems.insert(pDesktopItem->getUrl(), pDesktopItem);
    m_list_pItems.append(pDesktopItem);
    checkPageCount();

    if (!pDesktopItem.isNull()){
        GridItemPointer pGridItem = gridManager->getBlankItem();
        if (!pGridItem.isNull()){
            LOG_INFO() << pGridItem->getRow() << pGridItem->getColumn() << pGridItem->hasDesktopItem();
            pDesktopItem->move(pGridItem->getPos());
            pGridItem->setDesktopItem(true);
            m_settings.beginGroup("DesktopItems");
            m_settings.setValue(pDesktopItem->getUrl(), pDesktopItem->pos());
            m_settings.endGroup();
        }
    }
}

void DesktopItemManager::checkPageCount(){
    int rowCount = gridManager->getDesktopRowCount();
    int columnCount = gridManager->getDesktopColumnCount();
    int count = m_list_pItems.length();
    int pageCount =  count / (rowCount * columnCount) + 1;
    if (count % (rowCount * columnCount) > 0 && pageCount > 1){
        if (m_pagecount != pageCount){
            emit signalManager->pageCountChanged(pageCount);
            LOG_INFO() << pageCount << "pageCount" << "/////////////";
            m_pagecount = pageCount;
        }
    }
}

int DesktopItemManager::getPageCount(){
    return m_pagecount;
}

void DesktopItemManager::updateDesktopItemIcon(QString url, QString iconUrl, uint size){
    if (m_pItems.contains(url)){
        m_pItems.value(url)->setDesktopIcon(iconUrl);
    }
}


void DesktopItemManager::renameDesktopItem(DesktopItemInfo &desktopItemInfo){
    if (!m_shoudbeMovedItem.isNull()){
        QString desktopDisplayName = getDesktopDisplayName(desktopItemInfo);
        m_shoudbeMovedItem->setDesktopName(desktopDisplayName);
        m_shoudbeMovedItem->setDesktopItemInfo(desktopItemInfo);

        QString oldKey = m_shoudbeMovedItem->getUrl();
        if (m_pItems.contains(oldKey)){
            QString newKey = decodeUrl(desktopItemInfo.URI);
            QMap<QString, DesktopItemPointer>::iterator iterator = m_pItems.find(oldKey);
            if (iterator!= m_pItems.end()){
                m_shoudbeMovedItem->setUrl(newKey);
                m_pItems.insert(iterator, newKey, m_shoudbeMovedItem);

                m_settings.beginGroup("DesktopItems");
                if (m_settings.contains(oldKey)){
                    m_settings.remove(oldKey);
                    m_settings.setValue(newKey, m_shoudbeMovedItem->pos());
                }
                m_settings.endGroup();
            }
        }
    }
}

void DesktopItemManager::cutItems(QStringList urls){
    foreach (DesktopItemPointer pItem, m_list_pItems) {
        if (pItem->isCuted()){
            pItem->cancelCuted();
        }
    }
    foreach (QString url, urls) {
        if (m_pItems.contains(url)){
            m_pItems.value(url)->setCuted();
        }
    }
}

void DesktopItemManager::cancelCutedItems(QStringList urls){
    foreach (DesktopItemPointer pItem, m_list_pItems) {
        if (pItem->isCuted()){
            pItem->cancelCuted();
        }
    }
    LOG_INFO() << "cancel cuted";
}

void DesktopItemManager::deleteItem(QString url){
    QString _url = decodeUrl(url);

    if (m_pItems.contains(_url)){
        DesktopItemPointer pItem = m_pItems.value(_url);

        GridItemPointer pGridItem = gridManager->getItemByPos(pItem->pos());

        if (!pGridItem.isNull()){
            pGridItem->setDesktopItem(false);
        }

        if (m_list_pItems.contains(pItem)){
            m_list_pItems.removeOne(pItem);
        }
        m_pItems.remove(url);
        pItem->close();
    }
    checkPageCount();
    emit signalManager->desktopItemsSaved();
}


void DesktopItemManager::saveItems(){
    m_settings.clear();
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
}

DesktopItemPointer DesktopItemManager::getItemByPos(QPoint pos){
    foreach (DesktopItemPointer pItem, m_list_pItems) {
        if (pItem->geometry().contains(pos)){
            return pItem;
        }
    }
    return DesktopItemPointer();
}

QList<DesktopItemPointer> DesktopItemManager::getItems(){
    return m_list_pItems;
}

void DesktopItemManager::sortedByFlags(QDir::SortFlag flag){
    gridManager->clearDeskopItemsStatus();
    m_settings.clear();
    QDir desktopDir(desktopLocation);
    QFileInfoList desktopInfoList = desktopDir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot |QDir::Hidden, flag);
    m_list_pItems.clear();
    m_list_pItems.append(m_pComputerItem);
    m_list_pItems.append(m_pTrashItem);

    int row = gridManager->getRowCount();
    int size = desktopInfoList.size();
    for (int i = 0; i < size; i++) {
        QFileInfo fileInfo = desktopInfoList.at(i);
        QString url = "file://" + decodeUrl(fileInfo.absoluteFilePath());
        DesktopItemPointer  pDesktopItem = m_pItems.value(url);
        m_list_pItems.append(pDesktopItem);
    }

    for (int i = 0; i< m_list_pItems.length() ; i++){
        int pColumn = i / row;
        int pRow = i % row;
        GridItemPointer pGridItem = gridManager->getItems().at(pColumn)->at(pRow);
        if (!pGridItem.isNull()){
            QRect rect = pGridItem->getRect();
            LOG_INFO() << pRow << pColumn <<  rect << pGridItem->hasDesktopItem();
            m_list_pItems.at(i)->move(rect.topLeft());
            pGridItem->setDesktopItem(true);
        }
    }
    emit signalManager->desktopItemsSaved();
    m_sortFlag = flag;
}

void DesktopItemManager::sortedByKey(QString key){
    if (key == "name"){
        sortedByFlags(QDir::SortFlag::Name);
    }else if (key == "size"){
        sortedByFlags(QDir::SortFlag::Size);
    }else if (key == "filetype"){
        sortedByFlags(QDir::SortFlag::Type);
    }else if (key == "mtime"){

    }else if (key == "atime"){

    }else if (key == "open-with"){

    }else if (key == "tag-info"){

    }
}

void DesktopItemManager::resort(){
    sortedByFlags(m_sortFlag);
}

QDir::SortFlag DesktopItemManager::getSortFlag(){
    return m_sortFlag;
}

void DesktopItemManager::showAppGroupDetail(DesktopItemPointer &pItem, QPoint pos){
    if (m_appGroupBox){
        if (pItem->getUrl() == m_appGroupBox->property("url").toString()){
            closeAppGroupDetail();
            return;
        }
    }

    m_appGroupBox = new AppGroupBox();
    m_appGroupBox->showDetailByDesktopItem(pItem);
}

void DesktopItemManager::updateAppGroupDetail(DesktopItemPointer pItem){
    if (m_appGroupBox){
        if (m_appGroupBox->isVisible()){
            closeAppGroupDetail();
            m_appGroupBox = new AppGroupBox();
            m_appGroupBox->showDetailByDesktopItem(pItem);
        }
    }
}

void DesktopItemManager::closeAppGroupDetail(QPoint pos){
    if (m_appGroupBox){
        DesktopItemPointer pItem = getItemByPos(pos);
        m_appGroupBox->close();
        m_appGroupBox = NULL;
    }
}

void DesktopItemManager::closeAppGroupDetail(){
    if (m_appGroupBox){
        m_appGroupBox->close();
        m_appGroupBox = NULL;
    }
}

bool DesktopItemManager::isAppGroupBoxShowed(){
    if (m_appGroupBox){
        return m_appGroupBox->isVisible();
    }
    return false;
}

DesktopItemManager::~DesktopItemManager()
{

}
