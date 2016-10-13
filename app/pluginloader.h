/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef PLUGINLOADER_H
#define PLUGINLOADER_H

#include <QObject>

#include <QPluginLoader>

class PluginInterface;
class PluginLoader : public QObject
{
    Q_OBJECT
public:
    explicit PluginLoader(QObject *parent = 0);

signals:
    void pluginLoad(PluginInterface *);

public slots:
    void loadPlugins();

private:
    QList<QPluginLoader *> plugins;
};

#endif // PLUGINLOADER_H
