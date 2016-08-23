/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "desktopitemmanager.h"
#include "appgroupbox.h"
#include "global.h"
#include "controllers/dbuscontroller.h"
#include "desktopframe.h"

DesktopItemManager::DesktopItemManager(QObject *parent): QObject(parent)
{

    m_parentWindow = static_cast<DesktopFrame *>(this->parent());

    QSettings settings;
    settings.beginGroup("Desktop");
    int flag = settings.value("sortFlag", QDir::Name).toInt();
    settings.endGroup();
    m_sortFlag = static_cast<QDir::SortFlags>(flag);

    m_shoudbeMovedItem = DesktopItemPointer();

    m_gridUpdateTimer = new QTimer;
    m_gridUpdateTimer->setSingleShot(true);
    m_gridUpdateTimer->setInterval(100);

    initConnect();
}

void DesktopItemManager::initComputerItem()
{
    QString url = ComputerUrl;
    int width = gridManager->getItemWidth();
    int height = gridManager->getItemHeight();

    m_pComputerItem  = DesktopItemPointer::create(defaut_computerIcon, tr("Computer"), m_parentWindow);
    m_pComputerItem->setUrl(url);
    m_pComputerItem->setRaWUrl(url);
    m_pComputerItem->resize(width, height);
    m_pComputerItem->getDesktopItemInfo().URI = url;
    loadItemPosition(m_pComputerItem);
    m_pComputerItem->show();
}

void DesktopItemManager::initTrashItem()
{
    QString url = TrashUrl;
    int width = gridManager->getItemWidth();
    int height = gridManager->getItemHeight();
    m_pTrashItem  = DesktopItemPointer::create(defaut_trashIcon, tr("Trash"), m_parentWindow);
    m_pTrashItem->setUrl(url);
    m_pTrashItem->setRaWUrl(url);
    m_pTrashItem->resize(width, height);
    m_pTrashItem->getDesktopItemInfo().URI = url;
    m_pTrashItem->getDesktopItemInfo().CanExecute = true;
    loadItemPosition(m_pTrashItem);
    m_pTrashItem->show();
}


void DesktopItemManager::initConnect()
{
    connect(signalManager, SIGNAL(unCheckedItem(QString)), this, SLOT(unCheckedItem(QString)));
    connect(signalManager, SIGNAL(desktopItemsSaved()), this, SLOT(saveItemsPosition()));
    connect(signalManager, SIGNAL(desktoItemIconUpdated(QString, QString, uint)),
            this, SLOT(updateDesktopItemIcon(QString, QString, uint)));
    connect(signalManager, SIGNAL(sortedModeChanged(QDir::SortFlags)),
            this, SLOT(sortedByFlags(QDir::SortFlags)));
    connect(signalManager, SIGNAL(sortByKey(QString)),
            this, SLOT(sortedByKey(QString)));

    connect(signalManager, SIGNAL(gridOnResorted()), this, SLOT(resort()));
    connect(signalManager, SIGNAL(desktopItemsChanged(DesktopItemInfoMap)),
            this, SLOT(addItems(DesktopItemInfoMap)));
    connect(signalManager, SIGNAL(appGounpItemsChanged(QString, DesktopItemInfoMap)),
            this, SLOT(updateAppGounpItem(QString, DesktopItemInfoMap)));
    connect(signalManager, SIGNAL(appGounpDetailShowed(DesktopItemPointer &, QPoint)),
            this, SLOT(showAppGroupDetail(DesktopItemPointer &, QPoint)));
    connect(signalManager, SIGNAL(appGounpDetailClosed(QPoint)), this, SLOT(closeAppGroupDetail(QPoint)));
    connect(signalManager, SIGNAL(appGounpDetailClosed()), this, SLOT(closeAppGroupDetail()));


    connect(signalManager, SIGNAL(itemModify(QString, DesktopItemInfo)),
            this, SLOT(modifyItem(QString, DesktopItemInfo)));
    connect(signalManager, SIGNAL(itemCreated(DesktopItemInfo)),
            this, SLOT(addItem(DesktopItemInfo)));
    connect(signalManager, SIGNAL(itemDeleted(QString)),
            this, SLOT(deleteItem(QString)));

    connect(signalManager, SIGNAL(itemShoudBeMoved(QString)),
            this, SLOT(setShoudBeMovedItemByUrl(QString)));
    connect(signalManager, SIGNAL(itemMoved(DesktopItemInfo &)),
            this, SLOT(renameDesktopItem(DesktopItemInfo &)));

    connect(signalManager, SIGNAL(filesCuted(QStringList)),
            this, SLOT(cutItems(QStringList)));
    connect(signalManager, SIGNAL(cancelFilesCuted(QStringList)),
            this, SLOT(cancelCutedItems(QStringList)));
    connect(signalManager, SIGNAL(cancleFilesCuted()),
            this, SLOT(cancelCutedItems()));

    connect(signalManager, SIGNAL(dockModeChanged(int)),
            this, SLOT(handleDockModeChanged(int)));

    connect(signalManager, SIGNAL(fileCreated(QString)),
            this, SLOT(handleFileCreated(QString)));

    connect(m_gridUpdateTimer, SIGNAL(timeout()),
            this, SLOT(updateGridStatus()));

    connect(signalManager, SIGNAL(gridStatusUpdated()),
            this, SLOT(delayUpdateGridStatus()));

    connect(signalManager, SIGNAL(screenGeometryChanged()),
            this, SLOT(closeAppGroupDetail()));

    connect(signalManager, SIGNAL(pinyinResultChanged(QList<DesktopItemInfo>)),
            this, SLOT(handlePinyinChanged(QList<DesktopItemInfo>)));

    connect(signalManager, SIGNAL(fileMetaDataChanged(QString)),
            this, SLOT(handleFileMetaDataChanged(QString)));
}

void DesktopItemManager::updateItems(QString url, const DesktopItemPointer &pItem)
{
    m_pItems.insert(url, pItem);
    m_list_pItems.append(pItem);
    handleItemsChanged();
}

void DesktopItemManager::handleItemsChanged()
{
    if (gridManager->isFull()) {
        emit signalManager->rightBottomItemChangedtoBeContainer(true);
    } else {
        emit signalManager->rightBottomItemChangedtoBeContainer(false);
    }
}

void DesktopItemManager::handlePinyinChanged(const QList<DesktopItemInfo> &items)
{
    m_sortedPinyin_pItems = items;
}

void DesktopItemManager::handleFileMetaDataChanged(const QString &path)
{
    QString url = decodeUrl(path);
    if (m_pItems.contains(url)) {
        DesktopItemPointer pItem = m_pItems.value(url);
        handleDesktopItemMetaChanged(pItem);
        pItem->show();
    }
}

void DesktopItemManager::handleDesktopItemMetaChanged(const DesktopItemPointer &pItem)
{
    if (!pItem.isNull()) {
        QFileInfo info(pItem->getUrl());
        if (info.isSymLink()) {
            qDebug() << pItem->getUrl() << "is SymLink ";
        } else {
            pItem->setReadable(info.isReadable());

            bool isUserReadOnly = info.permission(QFile::ReadUser) && !info.permission(QFile::WriteUser) && !info.permission(QFile::ExeUser);
            pItem->setUserReadOnly(isUserReadOnly);

            bool isUserPermisson_000 = !info.permission(QFile::ReadUser) && !info.permission(QFile::WriteUser) && !info.permission(QFile::ExeUser);
            pItem->setUserReadPermisson_000(isUserPermisson_000);
        }
    }
}

void DesktopItemManager::unCheckedItem(QString url)
{
    DesktopItemPointer pItem = getItemByUrl(url);
    if (!pItem.isNull()) {
        pItem->setChecked(false);
    }
}

QString DesktopItemManager::getDesktopDisplayName(DesktopItemInfo &desktopItemInfo)
{
    QString desktopDisplayName;
    QString displayName = desktopItemInfo.DisplayName;
    if (displayName.startsWith(RichDirPrefix)) {
        int l = QString(RichDirPrefix).length();
        desktopDisplayName = displayName.remove(0, l);
    } else {
        desktopDisplayName = displayName;
    }
    return desktopDisplayName;
}

AppGroupBox *DesktopItemManager::getAppGroupBox()
{
    return m_appGroupBox;
}

DesktopItemPointer DesktopItemManager::getShoudBeMovedItem()
{
    return m_shoudbeMovedItem;
}

void DesktopItemManager::setShoudBeMovedItem(DesktopItemPointer pItem)
{
    m_shoudbeMovedItem = pItem;
}

void DesktopItemManager::setShoudBeMovedItemByUrl(QString url)
{
    QString key = url;
    if (m_pItems.contains(key)) {
        setShoudBeMovedItem(m_pItems.value(key));
    } else {
        qCritical() << "Invalid key" << key;
        setShoudBeMovedItem(DesktopItemPointer());
    }
}

void DesktopItemManager::addItems(DesktopItemInfoMap desktopInfoMap)
{
    QSettings setting;
    setting.beginGroup("DesktopItems");
    DesktopItemInfoMap unSavedDesktopItemInfoMap;
    for (int i = 0; i < desktopInfoMap.values().count(); i++) {
        QString url = decodeUrl(desktopInfoMap.values().at(i).URI);

        if (setting.contains(url)) {
            addItem(desktopInfoMap.values().at(i), i);
        } else {
            unSavedDesktopItemInfoMap.insert(desktopInfoMap.keys().at(i), desktopInfoMap.values().at(i));
        }
    }

    for (int i = 0; i < unSavedDesktopItemInfoMap.values().count(); i++) {
        addItem(unSavedDesktopItemInfoMap.values().at(i), i);
    }

    foreach(DesktopItemPointer pItem, m_pItems.values()) {
        pItem->show();
    }

}

void DesktopItemManager::updateAppGounpItem(QString group_url, DesktopItemInfoMap appItemInfos)
{
    QString key = decodeUrl(group_url);

    if (m_pItems.contains(key)) {
        m_pItems.value(key)->setAppGroupItems(appItemInfos);
        updateAppGroupDetail(m_pItems.value(key));
        if (static_cast<DesktopFrame *>(this->parent())->getAppGroupDestinationPos() != QPoint(-1, -1)) {
            m_pItems.value(key)->move(static_cast<DesktopFrame *>(this->parent())->getAppGroupDestinationPos());
            static_cast<DesktopFrame *>(this->parent())->setAppGroupDestinationPos(QPoint(-1, -1));
        }
    } else {
        qCritical() << "Invalid key" << key;
    }
}

DesktopItemPointer DesktopItemManager::createItem(DesktopItemInfo &fileInfo)
{
    emit signalManager->gridStatusUpdated();
    int width = gridManager->getItemWidth();
    int height = gridManager->getItemHeight();

    QString desktopDisplayName = getDesktopDisplayName(fileInfo);
    QString uri = fileInfo.URI;
    QString url = decodeUrl(uri);
    QString icon = fileInfo.Icon;

    DesktopItemPointer  pDesktopItem = DesktopItemPointer::create(m_parentWindow);
    pDesktopItem->setDesktopName(desktopDisplayName);
    pDesktopItem->setUrl(url);
    pDesktopItem->setRaWUrl(uri);
    pDesktopItem->setDesktopIcon(icon);
    pDesktopItem->setDesktopItemInfo(fileInfo);
    pDesktopItem->resize(width, height);

    handleDesktopItemMetaChanged(pDesktopItem);
    return pDesktopItem;
}


void DesktopItemManager::addItem(DesktopItemInfo fileInfo, int index)
{
    Q_UNUSED(index)
    DesktopItemPointer pDesktopItem = createItem(fileInfo);

    QString url = fileInfo.URI;
    if (url.endsWith(ComputerDesktop)) {
        if (!m_pItems.contains(ComputerUrl)) {
            initComputerItem();
        }
        return;
    }
    if (url.endsWith(TrashDesktop)) {
        if (!m_pItems.contains(TrashUrl)) {
            initTrashItem();
        }
        return;
    }

    if (!pDesktopItem.isNull()) {
        loadItemPosition(pDesktopItem);
    }
}

void DesktopItemManager::addItem(DesktopItemInfo fileInfo)
{
//    checkDesktopItemValid();
    if (m_pItems.contains(decodeUrl(fileInfo.URI))) {
        return;
    }
    DesktopItemPointer pDesktopItem = createItem(fileInfo);

    QString url = fileInfo.URI;
    if (url.endsWith(ComputerDesktop)) {
        if (!m_pItems.contains(ComputerUrl)) {
            initComputerItem();
        }
        return;
    }
    if (url.endsWith(TrashDesktop)) {
        if (!m_pItems.contains(TrashUrl)) {
            initTrashItem();
        }
        return;
    }

    if (!pDesktopItem.isNull()) {

        GridItemPointer pGridItem = gridManager->getBlankItem();

        if (!pGridItem.isNull()) {
            pDesktopItem->move(pGridItem->getPos());
            pDesktopItem->show();
            pGridItem->setDesktopItem(true);
            saveItemPosition(pDesktopItem);
        }
        updateItems(pDesktopItem->getUrl(), pDesktopItem);
    }
    checkDesktopItemValid();
}

void DesktopItemManager::modifyItem(QString url, DesktopItemInfo fileInfo)
{
    QString key;
    if (url == ComputerUrl || url == TrashUrl) {
        key = url;
    } else {
        key = decodeUrl(url);
    }

    if (m_pItems.contains(key)) {
        DesktopItemPointer pDesktopItem = m_pItems.value(key);

        QString desktopDisplayName = getDesktopDisplayName(fileInfo);
        QString uri = fileInfo.URI;
        QString url = decodeUrl(uri);
        QString icon = fileInfo.Icon;
        pDesktopItem->setDesktopName(desktopDisplayName);
        pDesktopItem->setUrl(url);
        pDesktopItem->setRaWUrl(uri);
        pDesktopItem->setDesktopIcon(icon);
        pDesktopItem->setDesktopItemInfo(fileInfo);
    } else {
        addItem(fileInfo);

        QTimer *refreshTimer = new QTimer;
        refreshTimer->setSingleShot(true);
        refreshTimer->setInterval(500);
        connect(refreshTimer, &QTimer::timeout, [ = ]() {
            emit signalManager->refreshCopyFileIcon(fileInfo.URI);
        });
        connect(refreshTimer, SIGNAL(timeout()), refreshTimer, SLOT(deleteLater()));
        refreshTimer->start();
    }
}

void DesktopItemManager::checkDesktopItemValid()
{
    foreach(QString url, m_pItems.keys()) {
        QFileInfo info(url);
        if (info.isSymLink()) {

        } else if (!info.exists() && url != ComputerUrl && url != TrashUrl) {
            qDebug() << "delete invalid desktop item " << url;
            deleteItem(url);
        }
    }
}

void DesktopItemManager::updateDesktopItemIcon(QString url, QString iconUrl, uint size)
{
    Q_UNUSED(size)
    QString key;
    if (url == ComputerUrl || url == TrashUrl) {
        key = url;
    } else {
        key = decodeUrl(url);
    }

    if (m_pItems.contains(key)) {
        m_pItems.value(key)->setDesktopIcon(iconUrl);

    }
}


void DesktopItemManager::renameDesktopItem(DesktopItemInfo &desktopItemInfo)
{

    if (!m_shoudbeMovedItem.isNull()) {
        QString desktopDisplayName = getDesktopDisplayName(desktopItemInfo);
        m_shoudbeMovedItem->setDesktopName(desktopDisplayName);
        m_shoudbeMovedItem->setDesktopItemInfo(desktopItemInfo);

        QString oldKey = m_shoudbeMovedItem->getUrl();
        if (m_pItems.contains(oldKey)) {
            QString newKey = decodeUrl(desktopItemInfo.URI);

            /*if desktopItem has exitsed, delete oldkey desktopItem*/
            if (m_pItems.contains(newKey)) {
                deleteItem(newKey);
            }
            QMap<QString, DesktopItemPointer>::iterator iterator = m_pItems.find(oldKey);
            if (iterator != m_pItems.end()) {
                m_shoudbeMovedItem->setUrl(newKey);
                m_shoudbeMovedItem->setRaWUrl(desktopItemInfo.URI);
                m_pItems.insert(iterator, newKey, m_shoudbeMovedItem);
                m_pItems.remove(oldKey);
                emit signalManager->fileMetaDataChanged(newKey);

                if (m_shoudbeMovedItem->isCuted()) {
                    QStringList files;
                    files.append(desktopItemInfo.URI);
                    emit signalManager->filesCuted(files);
                }

                GridItemPointer pGridItem = gridManager->getItemByPos(m_shoudbeMovedItem->pos());
                qDebug() << pGridItem.isNull();
                if (!pGridItem.isNull()) {
                    pGridItem->setDesktopItem(true);
                }

                m_settings.beginGroup("DesktopItems");
                if (m_settings.contains(oldKey)) {
                    m_settings.remove(oldKey);
                }
                m_settings.endGroup();

                saveItemPosition(m_shoudbeMovedItem);

                qDebug() << m_pItems.contains(newKey);
                emit signalManager->thumbnailRequested(newKey);
            }
        }
    } else {
        m_shoudbeMovedItem->deleteLater();
        addItem(desktopItemInfo);
    }
    checkDesktopItemValid();
}

void DesktopItemManager::cutItems(QStringList urls)
{
    foreach(DesktopItemPointer pItem, m_list_pItems) {
        if (pItem->isCuted()) {
            pItem->cancelCuted();
        }
    }
    foreach(QString url, urls) {
        url = decodeUrl(url);
        if (m_pItems.contains(url)) {
            m_pItems.value(url)->setCuted();
        }
    }
}

void DesktopItemManager::cancelCutedItems(QStringList urls)
{
    Q_UNUSED(urls)
    cancelCutedItems();
}

void DesktopItemManager::cancelCutedItems()
{
    qDebug() << "===============";
    foreach(DesktopItemPointer pItem, m_list_pItems) {
        if (pItem->isCuted()) {
            qDebug() << pItem->getDesktopName();
            pItem->cancelCuted();
        }
    }
    qDebug() << "cancel cuted";
}

void DesktopItemManager::deleteItem(QString url)
{
    if (url.endsWith(ComputerDesktop)) {
        deleteItem(ComputerUrl);
        return;
    }
    if (url.endsWith(TrashDesktop)) {
        deleteItem(TrashUrl);
        return;
    }

    QString _url(url);
//    if (url!=ComputerUrl && url!=TrashUrl){
//        if (!url.startsWith("file://")){
//            url = "file://" + url;
//        }
//        _url = decodeUrl(url);
//    }
    qDebug() << "deleteItem" << url << _url << m_pItems.contains(_url) << m_pItems.value(_url);
    if (m_pItems.contains(_url)) {
        DesktopItemPointer pItem = m_pItems.value(_url);

        GridItemPointer pGridItem = gridManager->getItemByPos(pItem->pos());

        if (!pGridItem.isNull()) {
            pGridItem->setDesktopItem(false);
        }

        if (m_list_pItems.contains(pItem)) {
            m_list_pItems.removeOne(pItem);
        }
        m_pItems.remove(_url);
        m_parentWindow->removeCheckedDesktopItem(pItem);
        pItem->close();
        pItem.clear();

        handleItemsChanged();
    }
//    checkPageCount();
    emit signalManager->desktopItemsSaved();
}

void DesktopItemManager::changeSizeByGrid(SizeType type)
{
    int width = gridManager->getItemWidth();
    int height = gridManager->getItemHeight();
    int row = gridManager->getRowCount();

    foreach(DesktopItemPointer pItem, m_list_pItems) {
        pItem->resize(width, height);
        pItem->updateSizeByGridSize(type);
        int i = m_list_pItems.indexOf(pItem) / row;
        int j = m_list_pItems.indexOf(pItem) % row;
        GridItemPointer pGridItem = gridManager->getItems().at(i)->at(j);
        if (!pGridItem.isNull()) {
            QRect rect = pGridItem->getRect();
            pItem->move(rect.topLeft());
            pGridItem->setDesktopItem(true);
        }
    }
}

DesktopItemPointer DesktopItemManager::getItemByPos(QPoint pos)
{
    foreach(DesktopItemPointer pItem, m_list_pItems) {
        if (pItem->pos() == pos && pos != gridManager->getRightBottomItem()->getPos()) {
            return pItem;
        }
    }
    return DesktopItemPointer();
}

DesktopItemPointer DesktopItemManager::getItemByUrl(QString url)
{
    if (m_pItems.contains(url)) {
        return m_pItems.value(url);
    }
    return DesktopItemPointer();
}

QList<DesktopItemPointer> DesktopItemManager::getItems()
{
    return m_list_pItems;
}

QList<DesktopItemPointer> DesktopItemManager::getRightBottomItems()
{
    QList<DesktopItemPointer> desktopItems;
    foreach(DesktopItemPointer pItem, m_list_pItems) {
        if (pItem->pos() == gridManager->getRightBottomItem()->getPos()) {
            desktopItems.append(pItem);
        }
    }
    return desktopItems;
}

QList<DesktopItemPointer> DesktopItemManager::getSortedItems()
{
    QList<DesktopItemPointer> desktopItems;
    for (int row = 0; row < gridManager->getItems().count(); row++) {
        GridListPointer plist = gridManager->getItems().at(row);
        for (int column = 0; column < plist->count(); column++) {
            GridItemPointer pGridItem = plist->at(column);
            if (!pGridItem.isNull()) {
                if (pGridItem == gridManager->getRightBottomItem()) {
                    desktopItems.append(getRightBottomItems());
                } else {
                    DesktopItemPointer pItem = getItemByPos(pGridItem->getPos());
                    if (!pItem.isNull()) {
                        desktopItems.append(pItem);
                    }
                }
            }
        }
    }
    return desktopItems;
}

QList<DesktopItemPointer> DesktopItemManager::getCheckedSortedItems()
{
    QList<DesktopItemPointer> desktopItems;
    for (int row = 0; row < gridManager->getItems().count(); row++) {
        GridListPointer plist = gridManager->getItems().at(row);
        for (int column = 0; column < plist->count(); column++) {
            GridItemPointer pGridItem = plist->at(column);
            if (!pGridItem.isNull()) {
//                if (pGridItem == gridManager->getRightBottomItem()){
//                    desktopItems.append(getRightBottomItems());
//                }else{
                DesktopItemPointer pItem = getItemByPos(pGridItem->getPos());
                if (!pItem.isNull()) {
                    if (pItem->isChecked()) {
                        desktopItems.append(pItem);
                    }
                }
//                }
            }
        }
    }
    return desktopItems;
}



QList<DesktopItemPointer> DesktopItemManager::getItemsByStartEnd(QPoint startPos, QPoint endPos)
{
    QList<DesktopItemPointer> items;
    int x1, x2, y1, y2;
    if (startPos.x() <= endPos.x()) {
        x1 = startPos.x();
        x2 = endPos.x();
    } else {
        x2 = startPos.x();
        x1 = endPos.x();
    }

    if (startPos.y() <= endPos.y()) {
        y1 = startPos.y();
        y2 = endPos.y();
    } else {
        y2 = startPos.y();
        y1 = endPos.y();
    }

    foreach(DesktopItemPointer pItem, m_list_pItems) {
        if (pItem->x() >= x1 && pItem->y() >= y1 && pItem->x() <= x2 && pItem->y() <= y2) {
            items.append(pItem);
        }
    }
    return items;
}

void DesktopItemManager::sortedByFlags(QDir::SortFlags flag)
{
    gridManager->clearDeskopItemsStatus();
    clearItemsPosition();

    QDir desktopDir(desktopLocation);
    QFileInfoList desktopInfoList = desktopDir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden | QDir::System, flag | QDir::DirsFirst);

    m_list_pItems.clear();

//    int row = gridManager->getRowCount();
    int size = desktopInfoList.size();
    for (int i = 0; i < size; i++) {
        QFileInfo fileInfo = desktopInfoList.at(i);
        QString url = decodeUrl(fileInfo.absoluteFilePath());
//        qDebug() << fileInfo.absoluteFilePath() << url << m_pItems.keys() << m_pItems.keys().contains(url) << m_pItems.keys().contains(fileInfo.absoluteFilePath());
        if (url.endsWith(ComputerDesktop)) {
            if (!m_pItems.contains(ComputerUrl)) {
                initComputerItem();
            }
            m_list_pItems.append(m_pComputerItem);
            continue;
        } else if (url.endsWith(TrashDesktop)) {
            if (!m_pItems.contains(TrashUrl)) {
                initTrashItem();
            }
            m_list_pItems.append(m_pTrashItem);
            continue;
        }

        if (m_pItems.contains(url)) {
            DesktopItemPointer  pDesktopItem = m_pItems.value(url);
            m_list_pItems.append(pDesktopItem);
        } else if (m_pItems.contains(fileInfo.absoluteFilePath())) {
            url = fileInfo.absoluteFilePath();
            DesktopItemPointer  pDesktopItem = m_pItems.value(url);
            m_list_pItems.append(pDesktopItem);
        } else {
            qCritical() << url;
        }
    }
    sortedItems();
    m_sortFlag = flag;
}

void DesktopItemManager::sortedItems()
{
    int columnCount = gridManager->getColumnCount();
    int rowCount = gridManager->getRowCount();
    int index = 0;
    bool isEnded = false;
    for (int column = 0; column < columnCount; column++) {
        if (isEnded) {
            break;
        }
        for (int row = 0; row < rowCount; row++) {
            index = column * gridManager->getRowCount() + row;
            GridItemPointer pGridItem = gridManager->getItems().at(column)->at(row);
            if (!pGridItem.isNull()) {
                QRect rect = pGridItem->getRect();
                if (index >= m_list_pItems.length()) {
                    isEnded = true;
                    break;
                }
                DesktopItemPointer pItem = m_list_pItems.at(index);
                if (!pItem.isNull()) {
                    pItem->move(rect.topLeft());
                    pGridItem->setDesktopItem(true);
                }
            }
        }
    }
    int startIndex = index + 1;
    if (startIndex < m_list_pItems.length()) {
        GridItemPointer pGridItem = gridManager->getRightBottomItem();
        for (int i = startIndex; i < m_list_pItems.length() ; i++) {
            DesktopItemPointer pItem = m_list_pItems.at(i);
            if (!pItem.isNull()) {
                if (!pGridItem.isNull()) {
                    QRect rect = pGridItem->getRect();
                    pItem->move(rect.topLeft());
                }
            }
        }
    }

    emit signalManager->desktopItemsSaved();
}

void DesktopItemManager::sortedByName()
{
    if (m_sortedPinyin_pItems.isEmpty()) {
        qDebug() << "m_sortedPinyin_pItems is empty, skip this sort operation.";
        return;
    }

    gridManager->clearDeskopItemsStatus();
    clearItemsPosition();
    m_list_pItems.clear();

    for (int i = 0; i < m_sortedPinyin_pItems.count(); i++) {
        QString URI = m_sortedPinyin_pItems.at(i).URI;
        QString url = decodeUrl(URI);

        if (url.endsWith(ComputerDesktop)) {
            if (!m_pItems.contains(ComputerUrl)) {
                initComputerItem();
            }
            m_list_pItems.append(m_pComputerItem);
            continue;
        } else if (url.endsWith(TrashDesktop)) {
            if (!m_pItems.contains(TrashUrl)) {
                initTrashItem();
            }
            m_list_pItems.append(m_pTrashItem);
            continue;
        }

        if (m_pItems.contains(url)) {
            m_list_pItems.append(m_pItems.value(url));
        }
    }
    sortedItems();
    m_sortFlag = QDir::Name;
}

void DesktopItemManager::sortedByKey(QString key){
    if (key == "name"){
        emit signalManager->sortedModeChanged(QDir::Name);
    }else if (key == "size"){
        emit signalManager->sortedModeChanged(QDir::Size);
    }else if (key == "filetype"){
        emit signalManager->sortedModeChanged(QDir::Type);
    }else if (key == "mtime"){
        emit signalManager->sortedModeChanged(QDir::Time | QDir::Reversed);
    }else if (key == "atime"){

    } else if (key == "open-with") {

    } else if (key == "tag-info") {

    }
}

void DesktopItemManager::resort()
{
    if (m_sortFlag == QDir::Name) {
        sortedByName();
    } else {
        sortedByFlags(m_sortFlag);
    }

}

QDir::SortFlags DesktopItemManager::getSortFlag()
{
    return m_sortFlag;
}

void DesktopItemManager::showAppGroupDetail(DesktopItemPointer &pItem, QPoint pos)
{
    Q_UNUSED(pos)
    if (m_appGroupBox) {
        if (pItem->getUrl() == m_appGroupBox->property("url").toString()) {
            closeAppGroupDetail();
            return;
        }
    }

    m_appGroupBox = new AppGroupBox(m_parentWindow);
    m_appGroupBox->showDetailByDesktopItem(pItem);
}

void DesktopItemManager::updateAppGroupDetail(DesktopItemPointer pItem)
{
    if (m_appGroupBox) {
        if (m_appGroupBox->isVisible()) {
            closeAppGroupDetail();
            m_appGroupBox = new AppGroupBox(m_parentWindow);
            m_appGroupBox->showDetailByDesktopItem(pItem);
        }
    }
}

void DesktopItemManager::closeAppGroupDetail(QPoint pos)
{
    if (m_appGroupBox) {
        DesktopItemPointer pItem = getItemByPos(pos);
        m_appGroupBox->close();
        m_appGroupBox = NULL;
    }
}

void DesktopItemManager::closeAppGroupDetail()
{
    if (m_appGroupBox) {
        qDebug() << "closeAppGroupDetail";
        m_appGroupBox->close();
        m_appGroupBox = NULL;
    }
}

bool DesktopItemManager::isAppGroupBoxShowed()
{
    if (m_appGroupBox) {
        return m_appGroupBox->isVisible();
    }
    return false;
}

void DesktopItemManager::handleDockModeChanged(int)
{
    /*
    if (dockMode == 0){
        qDebug() << "clearComputerTrashItems";
        clearComputerTrashItems();
    }else{
        qDebug() << "loadComputerTrashItems";
        loadComputerTrashItems();
    }
    */
}

void DesktopItemManager::handleFileCreated(QString filename)
{
    QString uri = decodeUrl(filename);
    QTimer *t = new QTimer;
    t->setSingleShot(true);
    t->setInterval(200);
    connect(t, &QTimer::timeout, this, [ = ]() {
        if (m_pItems.contains(uri)) {
            DesktopFrame *p = static_cast<DesktopFrame *>(this->parent());
            p->checkRaiseItem(m_pItems.value(uri));
            emit signalManager->requestRenamed(uri);
        }
    });
    t->start();
}

void DesktopItemManager::delayUpdateGridStatus()
{
    m_gridUpdateTimer->start();
}

void DesktopItemManager::updateGridStatus()
{
    gridManager->clearDeskopItemsStatus();
    QList<DesktopItemPointer> errorPositonItems;
    foreach(DesktopItemPointer pItem, m_pItems.values()) {
        foreach(GridItemPointer pGridItem, gridManager->getMapItems().values()) {
            if (pItem->pos() == pGridItem->getPos()) {
                if (!pGridItem->hasDesktopItem()) {
                    pGridItem->setDesktopItem(true);
                } else {
                    errorPositonItems.append(pItem);
                }
                break;
            }
        }
    }
    foreach(DesktopItemPointer pItem, errorPositonItems) {
        qDebug() << pItem->getDesktopName();
    }
}

DesktopItemManager::~DesktopItemManager()
{

}

static QString pointString(const QPoint &p)
{
    return QString("%1,%2").arg(p.x()).arg(p.y());
}

void DesktopItemManager::loadItemPosition(DesktopItemPointer pDesktopItem)
{
    QSettings setting;
    setting.beginGroup("DesktopItems");

    QPoint defaultPos;
    GridItemPointer blankItem = gridManager->getBlankItem();
    if (blankItem.isNull()) {
        defaultPos = gridManager->getRightBottomItem()->getPos();
    } else {
        defaultPos = blankItem->getPos();
    }

    QPoint pos = defaultPos;
    QString key = pDesktopItem->getUrl();
    QPoint index = setting.value(key, defaultPos).toPoint();
    if (gridManager->isVaildIndex(index.x(), index.y())) {
        pos = gridManager->mapIndexToPos(index.x(), index.y());
    } else {
        index = gridManager->mapPosToIndex(pos);
    }

    namePosBimap.insert(ItemPosPosition(key.toStdString(), pointString(index).toStdString()));
    if (namePosBimap.left.find(key.toStdString()) == namePosBimap.left.end()) {
        //insert failed
        qCritical() << "this is dump k-v in config" << index << key;
        pos = defaultPos;
        index = gridManager->mapPosToIndex(pos);
        namePosBimap.insert(ItemPosPosition(key.toStdString(), pointString(index).toStdString()));
    }

//    qDebug() << "move to" << index << pos << key;
    pDesktopItem->move(pos);
    GridItemPointer pGridItem = gridManager->getItemByPos(pos);
    if (!pGridItem.isNull()) {
        pGridItem->setDesktopItem(true);
    }
    setting.endGroup();

    updateItems(pDesktopItem->getUrl(), pDesktopItem);
}

bool DesktopItemManager::saveItemPosition(DesktopItemPointer pDesktopItem)
{
    m_settings.beginGroup("DesktopItems");
    QPoint index = gridManager->mapPosToIndex(pDesktopItem->pos());
    m_settings.setValue(pDesktopItem->getUrl(), index);
    m_settings.endGroup();
    return true;
}

void DesktopItemManager::saveItemsPosition()
{
    clearItemsPosition();
    m_settings.beginGroup("DesktopItems");
    foreach(DesktopItemPointer pItem, m_list_pItems) {
        if (!pItem.isNull()) {
            QPoint index = gridManager->mapPosToIndex(pItem->pos());
//            qDebug() << pItem->getDesktopName() << pItem->pos() << index;
            m_settings.setValue(pItem->getUrl(), index);
        }
    }
    m_settings.endGroup();
    m_settings.sync();
}

void DesktopItemManager::clearItemsPosition()
{
    m_settings.clear();
    namePosBimap.clear();
    m_settings.sync();
}
