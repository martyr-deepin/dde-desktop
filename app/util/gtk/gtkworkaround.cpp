/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/
extern "C" {
#include <gtk/gtk.h>
}

#include "gtkworkaround.h"

#include <QDebug>

static void requrestUpdateIcons(void)
{
    GtkIconTheme *gs = gtk_icon_theme_get_default();
    char* a = gtk_icon_theme_get_example_icon_name(gs);
    if (a != NULL) { g_free(a); }
    //can not passing QObject to the callback function,so use signal
    emit Gtk::GtkWorkaround::instance().gtkIconThemeChange(a);
}

void initGtkThemeWatcher()
{
    GtkIconTheme *gs = gtk_icon_theme_get_default();
    g_signal_connect(gs, "changed", requrestUpdateIcons, NULL);
    char* a = gtk_icon_theme_get_example_icon_name(gs);
    if (a != NULL) { g_free(a); }
}

void Gtk::Init()
{
    gtk_init(NULL, NULL);
    gdk_error_trap_push();
    initGtkThemeWatcher();
}

Gtk::GtkWorkaround::GtkWorkaround()
{
    qDebug() << "GtkWorkaround Init";
}
