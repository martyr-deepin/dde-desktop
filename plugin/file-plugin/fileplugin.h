/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#pragma once

#include <QObject>
#include <QtPlugin>

#include "file-plugin_global.h"

#include "../interface/interface.h"

class FILEPLUGINSHARED_EXPORT FilePlugin: public QObject, public PluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.deepin.Desktop.Plugin.FilePlugin" FILE "dde-desktop-file-plugin.json")
    Q_INTERFACES(PluginInterface)

public:
    FilePlugin();

    virtual void prepareData();

    virtual Widget* createWidget();

    virtual PresenterProxy*  presenterProxy();

    virtual bool registerDesktopMenu(const Desktop* desktop);

    virtual bool registerWidgetMenu(const Desktop* desktop);
};
