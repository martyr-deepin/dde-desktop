/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "pluginloader.h"

#include <QApplication>
#include <QLibraryInfo>
#include <QDir>
#include <QDebug>

#include "../plugin/interface/interface.h"
#include "presenter/widgetpresenter.h"
#include "desktop.h"

static const QString DesktopSystemPluginDir = "/usr/lib/dde-desktop/plugin";

PluginLoader::PluginLoader(QObject *parent) : QObject(parent)
{


}

void PluginLoader::loadPlugins()
{
    QDir pluginsDir(QApplication::applicationDirPath());
    if (!pluginsDir.cd("plugins")) {
        pluginsDir = QDir(DesktopSystemPluginDir);
    }

    qDebug() << "Load plugin from" << pluginsDir.absolutePath();

    const QStringList files = pluginsDir.entryList(QDir::Files | QDir::NoSymLinks);
    for (QString fileName : files) {
        if (!QLibrary::isLibrary(fileName)) {
            continue;
        }

        QString filePath = pluginsDir.absoluteFilePath(fileName);
        QPluginLoader pluginLoader(filePath);
        QJsonObject metaData = pluginLoader.metaData().value("MetaData").toObject();

        qDebug() << filePath
                 << metaData.value("id").toString()
                 << metaData.value("name").toString();

        auto plugin = qobject_cast<PluginInterface *>(pluginLoader.instance());

        Desktop::instance().presenter()->registerPresenterProxy(plugin->presenterProxy());

        emit pluginLoad(plugin);
    };
}
