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
#include "../global/singleton.h"

class DFileSystemModel;
class AppPresenter : public QObject, public Singleton<AppPresenter>
{
    Q_OBJECT
public:
    explicit AppPresenter(QObject *parent = 0);

    void init();

signals:
    void setConfig(const QString &group, const QString &key, const QVariant &value);
    void removeConfig(const QString &group, const QString &key);

public slots:
    void onSortRoleChanged(int role, Qt::SortOrder order);
    void onAutoAlignToggled();
    void OnIconLevelChanged(int iconLevel);

private:
    friend Singleton<AppPresenter>;
};

