/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef COPYJOBCONTROLLER_H
#define COPYJOBCONTROLLER_H

#include <QtCore>
#include <QtDBus>

class CopyJobInterface;
class CopyjobWorker;

class CopyJobController : public QObject
{
    Q_OBJECT
public:
    explicit CopyJobController(QObject *parent = 0);
    ~CopyJobController();

    void initConnect();

signals:

public slots:
    void createCopyJob(QStringList files, QString destination);
    void handleConflictConfirmResponse(const QMap<QString, QString>& jobDetail, const QMap<QString, QVariant>& response);
    void handleWokerTimerStop(const QMap<QString, QString>& jobDetail);
    void handleWokerTimerRestart(const QMap<QString, QString>& jobDetail);
private:
    QList<CopyjobWorker*> m_works;
};

#endif // COPYJOBCONTROLLER_H
