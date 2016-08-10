/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef FileMonitor_H
#define FileMonitor_H

#include <QtCore>

class FileMonitorWoker;

class FileMonitor : public QObject
{
    Q_OBJECT
public:
    explicit FileMonitor(QObject *parent = 0);
    ~FileMonitor();

    void initConnect();
    void initFileMonitorWoker();

    static bool isGoutputstreamTempFile(QString path);

signals:
    void fileCreated(QString path);
    void fileModify(QString path);
    void fileMovedIn(QString path);
    void fileMovedOut(QString out);
    void fileRenamed(QString oldPath, QString newPath);
    void fileDeleted(QString path);
    void fileMetaDataChanged(QString path);

    void appGroupUpdated(QString path);

public slots:
    void handleCreated(int cookie, QString path);
    void handleModify(int cookie, QString path);
    void handleMoveFrom(int cookie, QString path);
    void handleMoveTo(int cookie, QString path);
    void handleDelete(int cookie, QString path);
    void handleMetaDataChanged(int cookie, QString path);

    void delayHanleMoveFrom();
    void delayHandleAppGroupCreated();

private:
    QTimer* m_delayMoveOutTimer;
    QTimer* m_delayAppGroupUpdatedTimer;
    FileMonitorWoker* m_fileMonitorWorker;
    QMap<int, QString> m_moveEvent;
    QString m_appGroupPath;
};

#endif // FileMonitor_H
