/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef DBUSWORKER_H
#define DBUSWORKER_H

#include <QObject>

class QTimer;

class DBusWorker : public QObject
{
    Q_OBJECT
public:
    explicit DBusWorker(QObject *parent = 0);

signals:

public slots:
    void loadDesktopItems();
    void start();
    void stop();

private:
    QTimer* m_requestTimer;
    int m_count = 0;
};

#endif // DBUSWORKER_H
