/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include <dfileservices.h>
#include <dfmevent.h>
#include <QDebug>

#include "desktopitemcontrol.h"
#include "desktopiteminfo.h"

static inline const DUrl toFileUrl(const DUrl &desktopUrl)
{
    auto fileUrl = desktopUrl;
    fileUrl.setScheme("file");
    return fileUrl;
}

static inline const DUrlList toFileUrlList(const DUrlList &desktopUrlList)
{
    DUrlList fileUrlList;
    for (auto &url : desktopUrlList) {
        fileUrlList << toFileUrl(url);
    }
    return fileUrlList;
}

static inline const DFMEvent toFileEvent(const DFMEvent &event)
{
    DFMEvent desktopEvent = event;
    desktopEvent << toFileUrlList(event.fileUrlList());
    desktopEvent << toFileUrl(event.fileUrl());
    return desktopEvent;
}

DesktopItemControl::DesktopItemControl(QObject *parent) : DAbstractFileController(parent)
{

}

bool DesktopItemControl::openFile(const DUrl &desktopUrl, bool &accepted) const
{
    accepted = true;
    return DFileService::instance()->openFile(toFileUrl(desktopUrl));
}

bool DesktopItemControl::openFileByApp(const DUrl &desktopUrl, const QString &app, bool &accepted) const
{
    accepted = true;
    return DFileService::instance()->openFileByApp(toFileUrl(desktopUrl), app);
}

bool DesktopItemControl::compressFiles(const DUrlList &desktopUrlList, bool &accepted) const
{
    accepted = true;
    return DFileService::instance()->compressFiles(toFileUrlList(desktopUrlList));
}

bool DesktopItemControl::decompressFile(const DUrlList &desktopUrlList, bool &accepted) const
{
    accepted = true;
    return DFileService::instance()->decompressFile(toFileUrlList(desktopUrlList));
}

bool DesktopItemControl::decompressFileHere(const DUrlList &desktopUrlList, bool &accepted) const
{
    accepted = true;
    return DFileService::instance()->decompressFile(toFileUrlList(desktopUrlList));
}

bool DesktopItemControl::copyFilesToClipboard(const DUrlList &desktopUrlList, bool &accepted) const
{
    accepted = true;
    return DFileService::instance()->copyFilesToClipboard(toFileUrlList(desktopUrlList));
}

bool DesktopItemControl::renameFile(const DUrl &oldUrl, const DUrl &newUrl, bool &accepted) const
{
    accepted = true;
    return DFileService::instance()->renameFile(toFileUrl(oldUrl), toFileUrl(newUrl));
}

bool DesktopItemControl::deleteFiles(const DFMEvent &event, bool &accepted) const
{
    accepted = true;
    return DFileService::instance()->deleteFiles(toFileEvent(event));
}

DUrlList DesktopItemControl::moveToTrash(const DFMEvent &event, bool &accepted) const
{
    accepted = true;
    return DFileService::instance()->moveToTrashSync(toFileEvent(event));
}

bool DesktopItemControl::cutFilesToClipboard(const DUrlList &desktopUrlList, bool &accepted) const
{
    accepted = true;
    return DFileService::instance()->cutFilesToClipboard(toFileUrlList(desktopUrlList));
}

DUrlList DesktopItemControl::pasteFile(DAbstractFileController::PasteType type,
                                       const DUrl &targetUrl, const DFMEvent &event, bool &accepted) const
{
    // TODO: need return
    accepted = true;
    DFileService::instance()->pasteFile(type, toFileUrl(targetUrl), toFileEvent(event));
    return DUrlList();
}

bool DesktopItemControl::restoreFile(const DUrl &srcUrl, const DUrl &tarUrl, const DFMEvent &event, bool &accepted) const
{
    accepted = true;
    DFileService::instance()->restoreFile(toFileUrl(srcUrl), toFileUrl(tarUrl), toFileEvent(event));
    return true;
}

bool DesktopItemControl::newFolder(const DFMEvent &event, bool &accepted) const
{
    accepted = true;
    return DFileService::instance()->newFolder(toFileEvent(event));
}

bool DesktopItemControl::newFile(const DUrl &toUrl, bool &accepted) const
{
    accepted = true;
    return DFileService::instance()->newFile(toFileUrl(toUrl));
}

bool DesktopItemControl::newDocument(const DUrl &toUrl, bool &accepted) const
{
    accepted = true;
    return DFileService::instance()->newDocument(toFileUrl(toUrl));
}

bool DesktopItemControl::openFileLocation(const DUrl &desktopUrl, bool &accepted) const
{
    accepted = true;
    return DFileService::instance()->openFileLocation(toFileUrl(desktopUrl));
}

const QList<DAbstractFileInfoPointer> DesktopItemControl::getChildren(const DUrl &desktopUrl, const QStringList &nameFilters, QDir::Filters filters, QDirIterator::IteratorFlags flags, bool &accepted) const
{
    qDebug() << desktopUrl << nameFilters << filters;
    accepted = true;
    return DFileService::instance()->getChildren(toFileUrl(desktopUrl),
            nameFilters, filters, flags);
}

const DAbstractFileInfoPointer DesktopItemControl::createFileInfo(const DUrl &desktopUrl, bool &accepted) const
{
    qDebug() << desktopUrl;
    accepted = true;
    auto fileUrl = toFileUrl(desktopUrl);
//    if (fileUrl.toLocalFile().endsWith("dde-computer.desktop")) {
//        return DAbstractFileInfoPointer(new DesktopItemInfo(desktopUrl));
//    }
//    auto ret =  DFileService::instance()->createFileInfo(toFileUrl(desktopUrl));
    auto ret = DAbstractFileInfoPointer(new DesktopItemInfo(desktopUrl));
    qDebug() << ret->fileUrl();
    return ret;
}

const DDirIteratorPointer DesktopItemControl::createDirIterator(const DUrl &desktopUrl, const QStringList &nameFilters, QDir::Filters filters, QDirIterator::IteratorFlags flags, bool &accepted) const
{
    accepted = true;
    return DFileService::instance()->createDirIterator(toFileUrl(desktopUrl),
            nameFilters, filters, flags);
}

bool DesktopItemControl::createSymlink(const DUrl &desktopUrl, const DUrl &linkToUrl, bool &accepted) const
{
    accepted = true;
    return DFileService::instance()->createSymlink(toFileUrl(desktopUrl), toFileUrl(linkToUrl));
}

bool DesktopItemControl::unShareFolder(const DUrl &desktopUrl, bool &accepted) const
{
    accepted = true;
    return DFileService::instance()->unShareFolder(toFileUrl(desktopUrl));
}

bool DesktopItemControl::openInTerminal(const DUrl &desktopUrl, bool &accepted) const
{
    accepted = true;
    return DFileService::instance()->openInTerminal(toFileUrl(desktopUrl));
}

DAbstractFileWatcher *DesktopItemControl::createFileWatcher(const DUrl &desktopUrl, QObject *parent, bool &accepted) const
{
    accepted = true;
    return DFileService::instance()->createFileWatcher(toFileUrl(desktopUrl), parent);
}
