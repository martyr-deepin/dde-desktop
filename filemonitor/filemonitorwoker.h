/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef FILEMONITORWOKER_H
#define FILEMONITORWOKER_H

#include <QtCore>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/inotify.h>
#include <unistd.h>


#define RichDirPrefix ".deepin_rich_dir_"
#define desktopLocation QStandardPaths::standardLocations(QStandardPaths::DesktopLocation).at(0)

#define EVENT_NUM 16
#define MAX_BUF_SIZE 1024

class FileMonitorWoker : public QObject
{
    Q_OBJECT
public:
    explicit FileMonitorWoker(QObject *parent = 0);
    ~FileMonitorWoker();
    void handleInotifyEvent(int event, QString path);
    void addWatchFolder(const QString& path);
    void checkFilePath(const QString &path);

    void delWatchFile(const QString &path);
    void delWatchFolder(const QString &path);


    void addWatchGroup(const QString &path);
    void addWatchDir(const QString &path);
    void addWatchFile(const QString &path);
    void addSingleWatchFile(const QString &path);

    void checkingForChanges(const QString & path);
    void checkingFiles(const QString & path);
    void checkingDirs(const QString & path);

signals:
    void monitorFolderChanged(const QString& path);
    void fileCreated(int cookie, QString path);
    void fileMovedFrom(int cookie, QString path);
    void fileMovedTo(int cookie, QString out);
    void fileDeleted(int cookie, QString path);
    void metaDataChanged(int cookie, QString path);

public slots:
    void monitor(const QString& path);
    //void unMonitor(const QString& path);
    void handleDirectoryChanged(const QString & path);
    void handleFileChanged(const QString & path);

private:
    int m_fd;
    QMap<QString, int> m_path_wd;
    QMap<int, QString> m_wd_path;
    QFileSystemWatcher m_fileWatcher;
    QString m_currentFolder;
    QString m_desktopFolder;
    bool m_init_event;

};

#endif // FILEMONITORWOKER_H
