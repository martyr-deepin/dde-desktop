/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef DESKTOPAPP_H
#define DESKTOPAPP_H

#include <QtCore>
#include "views/gridmanager.h"


class DesktopBox;
class AppController;
class DTaskDialog;
class LauncherInterface;
class SchemaWatcher;

class DesktopApp : public QObject
{
    Q_OBJECT
public:
    explicit DesktopApp(QObject *parent = 0);
    ~DesktopApp();

    void initConnect();
    void registerDBusService();
    void createLauncerInterface();

signals:
    void closed();
    void shown();

public slots:
    void saveGridOn(bool mode);
    void saveSizeType(SizeType type);
    void saveSortFlag(QDir::SortFlags flag);

    void confimClear(int count);
    void confimDelete(const QMap<QString, QString> &files);
    void handleDeleteAction(int index);

    void confimConflict(const QMap<QString, QString>& jobDetail, const QMap<QString, QVariant>& response);
    void confirmRenameDialog(QString name);
    void chooseExecutableOpenOption(DesktopItemInfo info);

    void show();
    void hide();
    void toggle();
    void exit();

    void handleF1Pressed();

    void handleAppUninstalled(const QString& appKey);
    void removeInvalidDesktopFile(const QString& path, const QString& appKey);

private:
    DesktopBox* m_desktopBox;
    DTaskDialog* m_taskDialog;
    QDialog* m_renamDialog;
    QStringList m_deletefiles;
    LauncherInterface* m_launcherInterface;
    SchemaWatcher *m_schemaWatcher;
};

#endif // DESKTOPAPP_H
