/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef RENAMEJOBCONTROLLER_H
#define RENAMEJOBCONTROLLER_H

#include <QObject>

class RenameJobInterface;

class RenameJobController : public QObject
{
    Q_OBJECT
public:
    explicit RenameJobController(QObject *parent = 0);
    ~RenameJobController();

    void initConnect();

signals:

public slots:
    void rename(QString url, QString newName);
    void renameJobExcuteFinished(QString message);

private:
    RenameJobInterface* m_renameJobInterface = NULL;
    QString m_newName;
};

#endif // RENAMEJOBCONTROLLER_H
