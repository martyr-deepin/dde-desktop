/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include <QDebug>

#include "fileplugin.h"
#include "filewidget.h"

#include <QTimer>
#include <QLabel>

#include "../interface/presenterproxy.h"
#include "filewidget.h"
static PresenterProxy *sproxy = nullptr;

FilePlugin::FilePlugin()
{
    sproxy = new PresenterProxy;
    QTimer::singleShot(2000, this, [ = ]() {
        emit sproxy->addWidget(QVariant::fromValue(111), new FileWidget);
    });
}

void FilePlugin::prepareData()
{
    qDebug() << "prepareData";
}

Widget *FilePlugin::createWidget()
{
    qDebug() << "new widget";
    return  reinterpret_cast<Widget *>(new FileWidget) ;
}

PresenterProxy *FilePlugin::presenterProxy()
{
    qDebug() << "presenterProxy" << sproxy;
    return sproxy;
}

bool FilePlugin::registerDesktopMenu(const Desktop *desktop)
{
    qDebug() << desktop;
    return false;
}

bool FilePlugin::registerWidgetMenu(const Desktop *desktop)
{
    qDebug() << desktop;
    return false;
}
