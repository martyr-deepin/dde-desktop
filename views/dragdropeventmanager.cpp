#include "dragdropeventmanager.h"
#include "desktopframe.h"
#include "global.h"

DragDropEventManager::DragDropEventManager(QObject *parent) : QObject(parent)
{

}

void DragDropEventManager::handleDragMoveEvent(const QList<DesktopItemPointer>& items, QDragMoveEvent *event){
    DesktopFrame* m_parent = static_cast<DesktopFrame*>(parent());
    if (event->source() == m_parent){
        event->setDropAction(Qt::MoveAction);
        event->accept();
    }else{
        event->acceptProposedAction();
    }

    if (event->mimeData()->hasUrls()){
        QStringList urls;
        foreach (QUrl url, event->mimeData()->urls()) {
            urls.append(url.toString());
        }
        foreach(DesktopItemPointer pItem, items){
            if (pItem->geometry().contains(event->pos())){
                if (isAllApp(urls) && isApp(pItem->getUrl()) && !pItem->isChecked() && !pItem->isHover()){
                    m_hoverDesktopItem = pItem;
                    pItem->changeToBeAppGroupIcon();
                }
                pItem->setHover(true);
            }else{
                pItem->setHover(false);
                if (pItem == m_hoverDesktopItem){
                    pItem->changeBacktoNormal();
                }
            }
        }
    }
}


void DragDropEventManager::handleDropEvent(const QList<DesktopItemPointer>& items, QDropEvent *event){
    DesktopFrame* m_parent = static_cast<DesktopFrame*>(parent());
    if (event->source() == m_parent){
        event->setDropAction(Qt::MoveAction);
        event->accept();
    }else{
        event->acceptProposedAction();
    }

    bool flag = true;
    foreach(DesktopItemPointer pItem, items){
        if (pItem->geometry().contains(event->pos())){
            m_destinationDesktopItem = pItem;
            if (m_parent->isAllAppCheckedItems() && isApp(pItem->getUrl()) && !pItem->isChecked()){
                m_parent->setAppGroupDestinationPos(pItem->pos());
            }
            flag = flag && false;
            break;
        }else{
            flag = flag && true;
        }
    }
    if (flag){
        m_destinationDesktopItem.clear();
    }

    QStringList urls;
    if (event->mimeData()->hasUrls()){
        foreach (QUrl url, event->mimeData()->urls()) {
            urls.append(url.toEncoded());
        }
        qDebug() << event->mimeData()->urls() << urls;
        if (!m_destinationDesktopItem.isNull()){
            bool isCanMoved = !urls.contains(m_destinationDesktopItem->getRawUrl());
            qDebug() << isCanMoved << m_destinationDesktopItem->getRawUrl();
            if (isCanMoved){
                if (isAllApp(urls) && isApp(m_destinationDesktopItem->getUrl())){
                    qDebug() << "requestCreatingAppGroup";
                    urls.append(m_destinationDesktopItem->getUrl());
                    emit signalManager->requestCreatingAppGroup(urls);
                }else if (isAllApp(urls) && isAppGroup(m_destinationDesktopItem->getUrl())){
                    qDebug() << "requestMergeIntoAppGroup";
                    urls.append(m_destinationDesktopItem->getUrl());
                    emit signalManager->requestMergeIntoAppGroup(urls, m_destinationDesktopItem->getUrl());
                }else if (isApp(m_destinationDesktopItem->getUrl())){
                    qDebug() << "openFiles";
                    emit signalManager->openFiles(m_destinationDesktopItem->getDesktopItemInfo(), urls);
                }else if (isTrash(m_destinationDesktopItem->getUrl())){
                    qDebug() << "trashingAboutToExcute";
                    emit signalManager->trashingAboutToExcute(urls);
                }else if (isFolder(m_destinationDesktopItem->getUrl())){
                    qDebug() << "moveFilesExcuted";
                    emit signalManager->moveFilesExcuted(urls, m_destinationDesktopItem->getRawUrl());
                }
            }
        }else{
            if (event->source() != m_parent){
                emit signalManager->copyFilesExcuted(urls, desktopLocation);
            }/*else{
                qDebug() << "move desktop item";
            }*/
        }

    }
    m_destinationDesktopItem.clear();
}

DragDropEventManager::~DragDropEventManager()
{

}
