/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#pragma once

#include <dabstractfilecontroller.h>
#include "dabstractfileinfo.h"
#include "ddiriterator.h"

#include <QObject>
#include <QDir>
#include <QDirIterator>

class DFMEvent;
class DUrl;
class DAbstractFileWatcher;
typedef QList<DUrl> DUrlList;
class DesktopItemControl : public DAbstractFileController
{
    Q_OBJECT

public:
    explicit DesktopItemControl(QObject *parent = 0);

    virtual bool openFile(const DUrl &desktopUrl, bool &accepted) const;
    virtual bool openFileByApp(const DUrl &desktopUrl, const QString& app, bool &accepted) const;
    virtual bool compressFiles(const DUrlList &desktopUrlList, bool &accepted) const;
    virtual bool decompressFile(const DUrlList &desktopUrlList, bool &accepted) const;
    virtual bool decompressFileHere(const DUrlList &desktopUrlList, bool &accepted) const;
    virtual bool copyFilesToClipboard(const DUrlList &desktopUrlList, bool &accepted) const;
    virtual bool renameFile(const DUrl &oldUrl, const DUrl &newUrl, bool &accepted) const;
    virtual bool deleteFiles(const DFMEvent &event, bool &accepted) const;
    virtual DUrlList moveToTrash(const DFMEvent &event, bool &accepted) const;
    virtual bool cutFilesToClipboard(const DUrlList &desktopUrlList, bool &accepted) const;
    virtual DUrlList pasteFile(PasteType type, const DUrl &targetUrl, const DFMEvent &event, bool &accepted) const;
    virtual bool restoreFile(const DUrl &srcUrl, const DUrl &tarUrl, const DFMEvent &event, bool &accepted) const;
    virtual bool newFolder(const DFMEvent &event, bool &accepted) const;
    virtual bool newFile(const DUrl &toUrl, bool &accepted) const;
    virtual bool newDocument(const DUrl &toUrl, bool &accepted) const;
    virtual bool openFileLocation(const DUrl &desktopUrl, bool &accepted) const;

    virtual const QList<DAbstractFileInfoPointer> getChildren(const DUrl &desktopUrl, const QStringList &nameFilters,
                                                             QDir::Filters filters, QDirIterator::IteratorFlags flags,
                                                             bool &accepted) const;
    virtual const DAbstractFileInfoPointer createFileInfo(const DUrl &desktopUrl, bool &accepted) const;
    virtual const DDirIteratorPointer createDirIterator(const DUrl &desktopUrl, const QStringList &nameFilters,
                                                        QDir::Filters filters, QDirIterator::IteratorFlags flags,
                                                        bool &accepted) const;

    virtual bool createSymlink(const DUrl &desktopUrl, const DUrl &linkToUrl, bool &accepted) const;
    virtual bool unShareFolder(const DUrl &desktopUrl, bool &accepted) const;
    virtual bool openInTerminal(const DUrl &desktopUrl, bool &accepted) const;
    virtual DAbstractFileWatcher *createFileWatcher(const DUrl &desktopUrl, QObject *parent, bool &accepted) const;
};

