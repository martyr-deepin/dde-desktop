/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef TRASHJOBCONTROLLER_H
#define TRASHJOBCONTROLLER_H

#include <QtCore>
#include <QtDBus>

class TrashMonitorInterface;
class TrashJobInterface;
class EmptyTrashJobInterface;
class FileConflictController;

class TrashJobController : public QObject
{
    Q_OBJECT
public:
    explicit TrashJobController(QObject *parent = 0);
    ~TrashJobController();

    void initConnect();

signals:

public slots:
    void setTrashEmptyFlag(bool flag);
    void connectTrashSignal();
    void disconnectTrashSignal();
    void trashJobExcute(const QStringList& files);
    void trashJobExcuteFinished();
    void trashJobAbort();
    void trashJobAbortFinished();
    void onTrashingFile(QString file);
    void onDeletingFile(QString file);
    void onProcessAmount(qlonglong progress, ushort info);
    void updateTrashIconByCount(uint count);

    void confirmDelete();
    void handleTrashAction(int index);
    void createEmptyTrashJob();
    void emptyTrashJobExcuteFinished();

    void asyncRequestTrashCount();
    void asyncRequestTrashCountFinihsed(QDBusPendingCallWatcher *call);

private:
    TrashMonitorInterface* m_trashMonitorInterface=NULL;
    TrashJobInterface* m_trashJobInterface = NULL;
    EmptyTrashJobInterface* m_emptyTrashJobInterface = NULL;
    FileConflictController* m_conflictController = NULL;
    bool m_isTrashEmpty = true;
};

#endif // TRASHJOBCONTROLLER_H
