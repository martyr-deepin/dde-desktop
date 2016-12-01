/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "desktopiteminfo.h"

#include <QDebug>

static inline const DUrl toDesktopUrl(const DUrl &fileUrl)
{
    auto  desktopUrl  = fileUrl;
    desktopUrl.setScheme("desktop");
    desktopUrl.setHost("");
    return desktopUrl;
}

static inline const DUrlList toDesktopUrlList(const DUrlList &fileUrlList)
{
    DUrlList desktopUrlList;
    for (auto &url : fileUrlList) {
        desktopUrlList << toDesktopUrl(url);
    }
    return desktopUrlList;
}

DesktopItemInfo::DesktopItemInfo(const DUrl &url) : DAbstractFileInfo(url)
{
    auto ufurl= DUrl::fromLocalFile(url.path());
    m_proxy = new DFileInfo(ufurl);
    setProxy(DAbstractFileInfoPointer(m_proxy));

    qDebug() << this->exists() << url.path() << ufurl << m_proxy->exists();
}

DUrl DesktopItemInfo::symLinkTarget() const
{
    return toDesktopUrl(m_proxy->symLinkTarget());
}

DUrl DesktopItemInfo::fileUrl() const
{
    return toDesktopUrl(m_proxy->fileUrl());
}

DUrl DesktopItemInfo::parentUrl() const
{
    return toDesktopUrl(m_proxy->parentUrl());
}

DUrlList DesktopItemInfo::parentUrlList() const
{
    return toDesktopUrlList(m_proxy->parentUrlList());
}

DUrl DesktopItemInfo::redirectedFileUrl() const
{
    return toDesktopUrl(m_proxy->redirectedFileUrl());
}

DUrl DesktopItemInfo::getUrlByNewFileName(const QString &fileName) const
{
    return toDesktopUrl(m_proxy->getUrlByNewFileName(fileName));
}

DUrl DesktopItemInfo::getUrlByChildFileName(const QString &fileName) const
{
    return toDesktopUrl(m_proxy->getUrlByChildFileName(fileName));
}

DUrl DesktopItemInfo::mimeDataUrl() const
{
    return toDesktopUrl(m_proxy->fileUrl());
}

DUrl DesktopItemInfo::goToUrlWhenDeleted() const
{
    return toDesktopUrl(m_proxy->fileUrl());
}

