/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef DELETEJOBCONTROLLER_H
#define DELETEJOBCONTROLLER_H

#include <QObject>

#include <QtCore>
#include <QtDBus>

class DeleteJobInterface;

class DeleteJobController : public QObject
{
    Q_OBJECT
public:
    explicit DeleteJobController(QObject *parent = 0);
    ~DeleteJobController();

    void initConnect();

signals:

public slots:
    void createDeleteJob(const QStringList& files);
};

#endif // DELETEJOBCONTROLLER_H
