/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef DESKTOP_H
#define DESKTOP_H

#include <QObject>
#include <QScopedPointer>

class WidgetPresenter;
class DesktopPrivate;
class Desktop : public QObject
{
    Q_OBJECT
public:
    ~Desktop();

    static Desktop& instance() {
        static Desktop s_desktop;
        return s_desktop;
    }

    WidgetPresenter* presenter();

    void loadPlugins();
    void loadData();
signals:

public slots:
    void show();

private:
    explicit Desktop(QObject *parent = 0);
    Q_DISABLE_COPY(Desktop)

    QScopedPointer<DesktopPrivate> d;
};

#endif // DESKTOP_H
