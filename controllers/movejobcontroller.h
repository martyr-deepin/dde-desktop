/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef MOVEJOBCONTROLLER_H
#define MOVEJOBCONTROLLER_H


#include <QtCore>
#include <QtDBus>

class MoveJobInterface;
class MovejobWorker;

class MoveJobController : public QObject
{
    Q_OBJECT
public:
    explicit MoveJobController(QObject *parent = 0);
    ~MoveJobController();

    void initConnect();

signals:

public slots:
    void createMoveJob(QStringList files, QString destination);
    void handleConflictConfirmResponse(const QMap<QString, QString>& jobDetail, const QMap<QString, QVariant>& response);

private:
    QList<MovejobWorker*> m_works;
};

#endif // MOVEJOBCONTROLLER_H
