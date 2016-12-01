/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "apppresenter.h"

#include <dfileservices.h>

#include "../config/config.h"
#include "gridmanager.h"
#include "desktopitemcontrol.h"


#define DESKTOP_SCHEME "desktop"

AppPresenter::AppPresenter(QObject *parent) : QObject(parent)
{
}

void AppPresenter::init()
{
    connect(AppPresenter::instance(), &AppPresenter::setConfig,
            Config::instance(), &Config::setConfig);
    connect(AppPresenter::instance(), &AppPresenter::removeConfig,
            Config::instance(), &Config::removeConfig);

//    DFileService::dRegisterUrlHandler<DesktopItemControl>(DESKTOP_SCHEME, "");
}

void AppPresenter::onSortRoleChanged(int role, Qt::SortOrder order)
{
    emit setConfig(Config::groupGeneral, Config::keySortBy, role);
    emit setConfig(Config::groupGeneral, Config::keySortOrder, order);
}

void AppPresenter::onAutoAlignToggled()
{
    GridManager::instance()->toggleAlign();
    emit setConfig(Config::groupGeneral, Config::keyAutoAlign, GridManager::instance()->autoAlign());
}

void AppPresenter::OnIconLevelChanged(int iconLevel)
{
    emit setConfig(Config::groupGeneral, Config::keyIconLevel, iconLevel);
}

