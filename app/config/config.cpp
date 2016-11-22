/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/
#include "config.h"


#include <QThread>
#include <QStandardPaths>
#include <QApplication>
#include <QDebug>

Config::Config()
{
    auto configPath = QStandardPaths::standardLocations(QStandardPaths::ConfigLocation).first();
    configPath = configPath
                 + "/" + QApplication::organizationName()
                 + "/" + QApplication::applicationName()
                 + "/" + QApplication::applicationName() + ".conf";

    qDebug() << configPath;
    configPath = "/var/run/user/1000/desktop.conf";
    m_settings = new QSettings(configPath, QSettings::IniFormat);
    auto work = new QThread(this);
    this->moveToThread(work);
    work->start();
}

void Config::setItem(const QString &group, const QString &key, const QVariant &value)
{
    m_settings->beginGroup(group);
    m_settings->setValue(key, value);
    m_settings->endGroup();
    m_settings->sync();
}

void Config::removeItem(const QString &group, const QString &key)
{
    m_settings->beginGroup(group);
    m_settings->remove(key);
    m_settings->endGroup();
    m_settings->sync();
}
