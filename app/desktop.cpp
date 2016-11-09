/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "desktop.h"

#include "view/screenframe.h"
#include "presenter/widgetpresenter.h"
#include "pluginloader.h"

#include <QPushButton>
#include <QListView>
#include <QLabel>
#include <QTextEdit>
#include <QStringListModel>
#include <QStandardItemModel>

class DesktopPrivate
{
public:
    WidgetPresenter presenter;
    PluginLoader    pluginLoader;
    ScreenFrame     screenFrame;
};

Desktop::Desktop(QObject *parent)
    : QObject(parent), d(new DesktopPrivate)
{
}

Desktop::~Desktop()
{

}

WidgetPresenter *Desktop::presenter()
{
    return &d->presenter;
}

void Desktop::loadPlugins()
{
    d->pluginLoader.loadPlugins();
}

void Desktop::loadData()
{
    d->presenter.initModel(d->screenFrame.canvasSize());
}

void Desktop::show()
{
    d->screenFrame.show();
}
