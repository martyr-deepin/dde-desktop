/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "dragdropeventmanager.h"
#include "desktopframe.h"
#include "global.h"
#include "xdnd/xdndworkaround.h"
#include <QDebug>
#include <QX11Info>
#include <QMimeData>
#include <xcb/xproto.h>

DragDropEventManager::DragDropEventManager(QObject *parent) : QObject(parent)
{


}

void DragDropEventManager::handleDragMoveEvent(const QList<DesktopItemPointer>& items, const QList<QUrl> &urls, const QPoint &pos){
    QStringList stringUrls;
    foreach (QUrl url, urls) {
        stringUrls.append(url.toString());
    }
    foreach(DesktopItemPointer pItem, items){
        if (pItem->geometry().contains(pos)){
            /* app group
            if (isAllApp(stringUrls) && isApp(pItem->getUrl()) && !pItem->isChecked() && !pItem->isHover()){
                m_hoverDesktopItem = pItem;
                pItem->changeToBeAppGroupIcon();
            }
            */

            pItem->setHover(true);
        }else{
            pItem->setHover(false);

            /* app group
            if (pItem == m_hoverDesktopItem){
                pItem->changeBacktoNormal();
            }
            */
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

//        qDebug() << event->mimeData()->urls() << urls;
        if (!m_destinationDesktopItem.isNull()){
            bool isCanMoved = !urls.contains(m_destinationDesktopItem->getRawUrl());
            qDebug() << isCanMoved << m_destinationDesktopItem->getRawUrl();
            if (isCanMoved){
                /* app group
                if (isAllApp(urls) && isApp(m_destinationDesktopItem->getUrl())){
                    qDebug() << "requestCreatingAppGroup";
                    urls.append(m_destinationDesktopItem->getUrl());
                    emit signalManager->requestCreatingAppGroup(urls);
                }else if (isAllApp(urls) && isAppGroup(m_destinationDesktopItem->getUrl())){
                    qDebug() << "requestMergeIntoAppGroup";
                    urls.append(m_destinationDesktopItem->getUrl());
                    emit signalManager->requestMergeIntoAppGroup(urls, m_destinationDesktopItem->getUrl());
                }else
                */
                if (isApp(m_destinationDesktopItem->getUrl())){
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

            if (event->mimeData()->data("source") == "AppTableWidget"){
                emit signalManager->moveFilesExcuted(urls, desktopLocation);
            }else{
                if (event->source() != m_parent){
                    emit signalManager->copyFilesExcuted(urls, desktopLocation);
                }else{
                    qDebug() << "move desktop item";
                }
            }
        }

    }

        switch(event->dropAction()){
        case Qt::CopyAction:
        static_cast<DragDropEventManager*>(parent())->childDropEvent(event);
        }

    m_destinationDesktopItem.clear();
}

void DragDropEventManager::childDropEvent(QDropEvent* e)
{
    qDebug("drop");		   // qDebug("drop");
    // Try to support XDS
    // NOTE: in theory, it's not possible to implement XDS with pure Qt.
    // We achieved this with some dirty XCB/XDND workarounds.
    // Please refer to XdndWorkaround::clientMessage() in xdndworkaround.cpp for details.
    if(QX11Info::isPlatformX11() && e->mimeData()->hasFormat("XdndDirectSave0")) {
      e->setDropAction(Qt::CopyAction);
  //    const QWidget* targetWidget = childView()->viewport();
      // these are dynamic QObject property set by our XDND workarounds in xworkaround.cpp.
  //    xcb_window_t dndSource = xcb_window_t(targetWidget->property("xdnd::lastDragSource").toUInt());
  //    xcb_timestamp_t dropTimestamp = (xcb_timestamp_t)targetWidget->property("xdnd::lastDropTime").toUInt();

      qDebug() << XdndWorkaround::lastDragSource;
      qDebug() << XdndWorkaround::lastDropTime;
      xcb_window_t dndSource = xcb_window_t(XdndWorkaround::lastDragSource);
      xcb_timestamp_t dropTimestamp = (xcb_timestamp_t)XdndWorkaround::lastDropTime;
      // qDebug() << "XDS: source window" << dndSource << dropTimestamp;
      if(dndSource != 0) {
        xcb_connection_t* conn = QX11Info::connection();
        xcb_atom_t XdndDirectSaveAtom = XdndWorkaround::internAtom("XdndDirectSave0", 15);
        xcb_atom_t textAtom = XdndWorkaround::internAtom("text/plain", 10);

        // 1. get the filename from XdndDirectSave property of the source window


        QByteArray basename = XdndWorkaround::windowProperty(dndSource, XdndDirectSaveAtom, textAtom, 1024);

        // 2. construct the fill URI for the file, and update the source window property.

        QByteArray fileUri;
        fileUri.append(FilePrefix + desktopLocation + "/" + basename);
        XdndWorkaround::setWindowProperty(dndSource,  XdndDirectSaveAtom, textAtom, (void*)fileUri.constData(), fileUri.length());

        // 3. send to XDS selection data request with type "XdndDirectSave" to the source window and
        //    receive result from the source window. (S: success, E: error, or F: failure)
        QByteArray result = e->mimeData()->data("XdndDirectSave0");
        qDebug() <<"result" << result;
        // NOTE: there seems to be some bugs in file-roller so it always replies with "E" even if the
        //       file extraction is finished successfully. Anyways, we ignore any error at the moment.
      }
      e->accept(); // yeah! we've done with XDS so stop Qt from further event propagation.
      return;
    }
}
DragDropEventManager::~DragDropEventManager()
{

}
