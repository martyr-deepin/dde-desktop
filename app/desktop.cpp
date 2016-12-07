/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "desktop.h"

#include "view/canvasgridview.h"
#include "presenter/apppresenter.h"

#include <QPushButton>
#include <QListView>
#include <QLabel>
#include <QTextEdit>
#include <QStringListModel>
#include <QStandardItemModel>
#include <QStandardPaths>
#include <QDebug>

#include <durl.h>

class DesktopPrivate
{
public:
    AppPresenter        presenter;
    CanvasGridView      screenFrame;
};

Desktop::Desktop()
    : d(new DesktopPrivate)
{

}

Desktop::~Desktop()
{

}

void Desktop::loadData()
{
    d->presenter.init();
}

void Desktop::loadView()
{
    auto desktopPath = QStandardPaths::standardLocations(QStandardPaths::DesktopLocation).first();
    auto desktopUrl = DUrl::fromLocalFile(desktopPath);
    d->screenFrame.setRootUrl(desktopUrl);
}

void Desktop::Show()
{
    d->screenFrame.show();
}
