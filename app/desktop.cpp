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
#include "presenter/canvasgridpresenter.h"

#include <QPushButton>
#include <QListView>
#include <QLabel>
#include <QTextEdit>
#include <QStringListModel>
#include <QStandardItemModel>

class DesktopPrivate
{
public:
    CanvasGridPresenter presenter;
    ScreenFrame     screenFrame;
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

}

void Desktop::show()
{
    d->screenFrame.show();
}
