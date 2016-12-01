/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#pragma once

#include <dfileinfo.h>

class DFMEvent;
class DesktopItemInfoPrivate;
class DesktopItemInfo : public DAbstractFileInfo
{
public:
    explicit DesktopItemInfo(const DUrl &url);

    DUrl symLinkTarget() const Q_DECL_OVERRIDE;
    DUrl fileUrl() const Q_DECL_OVERRIDE;
    DUrl parentUrl() const Q_DECL_OVERRIDE;
    DUrlList parentUrlList() const Q_DECL_OVERRIDE;
    DUrl redirectedFileUrl() const Q_DECL_OVERRIDE;
    DUrl getUrlByNewFileName(const QString &fileName) const Q_DECL_OVERRIDE;
    DUrl getUrlByChildFileName(const QString &fileName) const Q_DECL_OVERRIDE;
    DUrl mimeDataUrl() const Q_DECL_OVERRIDE;
    DUrl goToUrlWhenDeleted() const Q_DECL_OVERRIDE;

private:
    Q_DECLARE_PRIVATE(DesktopItemInfo)

    DFileInfo *m_proxy;
};
