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
#include <QSettings>

#include "../global/singleton.h"

class Config: public QObject, public Singleton<Config>
{
    Q_OBJECT
public:
    QSettings *settings()  {return m_settings;}

public slots:
    void setItem(const QString &group, const QString &key, const QVariant &value);
    void removeItem(const QString &group, const QString &key);

private:
    Q_DISABLE_COPY(Config)
    explicit Config();
    friend Singleton<Config>;

    QSettings   *m_settings = nullptr;
};
