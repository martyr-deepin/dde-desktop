/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef MOVEJOBWORKER_H
#define MOVEJOBWORKER_H

#include <QObject>
#include <QTimer>
#include <QStringList>
#include <QString>
#include <QTime>
#include <QMap>
#include "dbusinterface/dbustype.h"

class MoveJobInterface;
class FileConflictController;

class MovejobWorker : public QObject
{
    Q_OBJECT
public:
    explicit MovejobWorker(QStringList files, QString destination, QObject *parent = 0);
    void initConnect();
    QStringList getFiles();
    QString getDestination();
    QString getJobPath();
    const QMap<QString, QString>& getJobDetail();
    FileConflictController* getFileConflictController();

signals:
    void startJob();
    void finished();

public slots:
    void start();
    void moveFiles(QStringList files, QString destination);
    void connectMoveJobSignal();
    void disconnectMoveJobSignal();
    void moveJobExcuteFinished(const QString& message);
    void moveJobAbort();
    void moveJobAbortFinished();
    void onMovingFile(QString file);
    void setTotalAmount(qlonglong amount, ushort type);
    void onMovingProcessAmount(qlonglong progress, ushort info);
    void handleTimeout();
    void handleFinished();
    void handleTaskAborted(const QMap<QString, QString>& jobDetail);
    void handleResponse(ConflictInfo obj);
private:
    QStringList m_files;
    QString m_destination;
    QString m_movejobPath;
    MoveJobInterface* m_moveJobInterface = NULL;
    FileConflictController* m_conflictController = NULL;
    QTimer* m_progressTimer;
    QMap<QString, QString> m_jobDetail;
    QMap<QString, QString> m_jobDataDetail;
    qlonglong m_totalAmout;
    QTime* m_time;
    qlonglong m_lastProgress = 0;
    qlonglong m_currentProgress = 0;
    int m_elapsedTime;
};

#endif // MOVEJOBWORKER_H
