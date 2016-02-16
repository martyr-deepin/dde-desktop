/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef THEMEAPPICON_H
#define THEMEAPPICON_H

#include <QObject>

class ThemeAppIcon : public QObject
{
    Q_OBJECT

public:
    explicit ThemeAppIcon(QObject *parent = 0);
    ~ThemeAppIcon();

    void gtkInit();

    static QPixmap getIconPixmap(QString iconPath, int width=48, int height=48);
    static QString getThemeIconPath(QString iconName);

signals:

public slots:
};

#endif // THEMEAPPICON_H
