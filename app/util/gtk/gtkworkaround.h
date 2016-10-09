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

namespace Gtk
{
void Init();


class GtkWorkaround: public QObject
{
    Q_OBJECT
public:
    GtkWorkaround();

    static GtkWorkaround &instance()
    {
        static GtkWorkaround gw;
        return gw;
    }

signals:
    void gtkIconThemeChange(const QString &iconTheme);

private:
    Q_DISABLE_COPY(GtkWorkaround)

};

}
