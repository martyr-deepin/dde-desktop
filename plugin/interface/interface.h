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

class Desktop;
class PresenterProxy;
class Widget;

class PluginInterface
{
public:
    virtual ~PluginInterface();

    virtual void prepareData() = 0;

    virtual Widget* createWidget() = 0;

    virtual PresenterProxy* presenterProxy() = 0;

    virtual bool registerDesktopMenu(const Desktop* desktop) = 0;

    virtual bool registerWidgetMenu(const Desktop* desktop) = 0;
};

#define DesktopPluginInterface_iid "com.deepin.Desktop.PluginInterface"

Q_DECLARE_INTERFACE(PluginInterface, DesktopPluginInterface_iid)
